#include "./common.h"
#include <arpa/inet.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <format>
#include <future>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <string>
#include <string_view>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <tuple>
#include <unistd.h>

using namespace std;

// use fixed port for now
const uint16_t PORT = 8080;
const size_t BUFFER_SIZE = 1024;
const chrono::seconds MESSAGE_PERIOD = chrono::seconds(1); // unit: second
const chrono::seconds ZERO_SEC = chrono::seconds(0);       // unit: second

const string_view USAGE_MSG =
    "Usage: ./client {ip_address, e.g. 127.0.0.1} {port, e.g. 8080} {client "
    "id, non-zero positive integer}";

auto send_message(int socketfd, string_view message) -> bool {
  auto ret = send(socketfd, message.data(), message.size(), 0);
  if (ret == -1) {
    perror("Send failed, Error");
    return false;
  }
  return true;
}

auto send_message_and_wait(int socketfd, string_view message,
                           chrono::seconds wait_time) {
  send_message(socketfd, message);
  this_thread::sleep_for(wait_time);
  return;
}

auto receive_message(int socketfd) -> optional<string> {
  array<char, BUFFER_SIZE> buffer{};
  buffer.fill(0);
  ssize_t bytes_received = recv(socketfd, buffer.data(), BUFFER_SIZE, 0);
  if (bytes_received <= 0) {
    perror("Server disconnected or error occurred. Error");
    return nullopt;
  }
  auto response = string(buffer.begin(), buffer.end());
  return response;
}

auto parse_cli_options(int argc, char **argv)
    -> std::optional<std::tuple<sockaddr_in, uint>> {
  if (argc != 4) {
    return std::nullopt;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;

  auto ret = inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
  if (ret == -1) {
    std::perror("Parse address failed, Error");
    return std::nullopt;
  }

  auto port = atoi(argv[2]);
  if (port == 0) {
    return std::nullopt;
  }
  server_addr.sin_port = htons(port);

  auto client_id = atoi(argv[3]);
  if (client_id == 0) {
    return std::nullopt;
  }

  return make_tuple(server_addr, client_id);
}

int main(int argc, char **argv) {
  auto server_addr_opt = parse_cli_options(argc, argv);
  if (!server_addr_opt.has_value()) {
    cout << USAGE_MSG;
    return 1;
  }
  const auto [server_addr, client_id] = server_addr_opt.value();

  // Create socket
  // use IPPROTO_TCP instead of 0, because we only care about tcp connection.
  FileDescriptor client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket == -1) {
    perror("Failed to create socket, Error");
    return 1;
  }

  const sockaddr *addr = reinterpret_cast<const sockaddr *>(&server_addr);
  if (connect(client_socket, addr, sizeof(server_addr)) == -1) {
    perror("Connection failed, Error");
    return 1;
  }
  cout << "Connected to server" << endl;

  uint8_t message_count = 0;

  auto send_msg = [&client_socket, &message_count, &client_id]() {
    auto message = std::format("This is message {} from client {}",
                               message_count, client_id);
    return send_message_and_wait(client_socket, message, MESSAGE_PERIOD);
  };
  auto receive_msg = [&client_socket]() {
    return receive_message(client_socket);
  };

  auto send_msg_future = async(launch::async, send_msg);
  auto receive_msg_future = async(launch::async, receive_msg);

  while (true) {
    // Check if it can send another message
    if (send_msg_future.wait_for(ZERO_SEC) == future_status::ready) {
      message_count += 1;
      send_msg_future = async(launch::async, send_msg);
    }

    // Check if it receive any message
    if (receive_msg_future.wait_for(ZERO_SEC) == future_status::ready) {
      auto response = receive_msg_future.get();
      // Server disconnected
      if (!response.has_value()) {
        break;
      }
      cout << "Server response: " << response.value() << endl;
      receive_msg_future = async(launch::async, receive_msg);
    }
  }

  return 0;
}
