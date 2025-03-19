#include <arpa/inet.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <future>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <string_view>
#include <strings.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace std;

// use fixed port for now
const uint16_t PORT = 8080;
const size_t BUFFER_SIZE = 16;
const chrono::seconds MESSAGE_PERIOD = chrono::seconds(1); // unit: second
const chrono::seconds ZERO_SEC = chrono::seconds(0);       // unit: second

const string_view USAGE_MSG = "Usage: ./client {ip_address} {port}";

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

auto parse_server_addr(int argc, char **argv) -> std::optional<sockaddr_in> {
  if (argc != 3) {
    return std::nullopt;
  }

  sockaddr_in server_addr{.sin_family = AF_INET,
                          .sin_port = htons(atoi(argv[2])),
                          .sin_addr = {.s_addr = {}}};

  auto ret = inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
  if (ret == -1) {
    std::perror("Parse address failed, Error");
    return std::nullopt;
  }
  return server_addr;
}

int main(int argc, char **argv) {
  auto server_addr_opt = parse_server_addr(argc, argv);
  if (!server_addr_opt.has_value()) {
    cout << argv[0] << USAGE_MSG;
    return 1;
  }
  auto server_addr = server_addr_opt.value();

  // Create socket
  // use IPPROTO_TCP instead of 0, because we only care about tcp connection.
  auto sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    perror("Failed to create socket, Error");
    return 1;
  }

  sockaddr *addr = reinterpret_cast<sockaddr *>(&server_addr);
  if (connect(sockfd, addr, sizeof(server_addr)) == -1) {
    perror("Connection failed, Error");
    return 1;
  }
  cout << "Connected to server" << endl;

  string message = "abcdefghijklmnopqrstuvwxyz";

  // Non-blocking way to send and receive message
  auto send_msg = async(launch::async, send_message_and_wait, sockfd, message,
                        MESSAGE_PERIOD);
  auto receive_msg = async(launch::async, receive_message, sockfd);

  while (true) {
    if (send_msg.wait_for(ZERO_SEC) == future_status::ready) {
      send_msg = async(launch::async, send_message_and_wait, sockfd, message,
                       MESSAGE_PERIOD);
    }

    if (receive_msg.wait_for(ZERO_SEC) == future_status::ready) {
      auto response = receive_msg.get();
      // Server disconnected
      if (!response.has_value()) {
        break;
      }
      cout << "Server response: " << response.value() << endl;
      receive_msg = async(launch::async, receive_message, sockfd);
    }
  }

  close(sockfd);
  return 0;
}
