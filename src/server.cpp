#include "./common.h"
#include <arpa/inet.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <thread>

// Listen to loopback address ((127.0.0.1)) only.
// Listen to all address use INADDR_ANY instead of INADDR_LOOPBACK
const auto LISTEN_ADDR = INADDR_LOOPBACK;
const uint16_t PORT = 8080;

const size_t BUFFER_SIZE = 1024;
const int QUEUE_BACKLOG = 5;

void handle_client(FileDescriptor &&client_socket) {
  std::array<char, BUFFER_SIZE> buffer{};
  while (true) {
    ssize_t bytes_received = recv(client_socket, buffer.data(), BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      std::perror("Client disconnected or error occurred. Error");
      return;
    }
    auto message = std::string(buffer.begin(), buffer.end());
    std::cout << "Received: " << message << std::endl;

    // echo back the message
    send(client_socket, buffer.data(), bytes_received, 0);
  }
}

int main(int argc, char **argv) {
  if (argc != 1) {
    std::cout << argv[0] << "takes no arguments.\n";
    return 1;
  }

  // Create socket
  // use IPPROTO_TCP instead of 0, because we only care about tcp connection.
  FileDescriptor server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket == -1) {
    std::perror("Failed to create socket, Error");
    return 1;
  }

  sockaddr_in server_addr{.sin_family = AF_INET,
                          .sin_port = htons(PORT),
                          .sin_addr = {.s_addr = htonl(LISTEN_ADDR)}};
  sockaddr *server_addr_ptr = reinterpret_cast<sockaddr *>(&server_addr);

  if (bind(server_socket, server_addr_ptr, sizeof(server_addr)) == -1) {
    std::perror("Bind failed, Error");
    return 1;
  }

  if (listen(server_socket, QUEUE_BACKLOG) == -1) {
    std::perror("Listen failed, Error");
    return 1;
  }

  std::cerr << "Server listening on port " << PORT << "..." << std::endl;

  // Accept the data packet from client and verification
  while (true) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    sockaddr *client_addr_ptr = reinterpret_cast<sockaddr *>(&server_addr);

    int client_socket = accept(server_socket, client_addr_ptr, &client_len);
    if (client_socket == -1) {
      std::perror("Accept failed, Error");
      continue;
    }
    // Create thread for each client, to handle them concurrently.
    std::thread(handle_client, std::move(client_socket)).detach();
    std::cout << "Client connected" << std::endl;
  }

  return 0;
}
