#include <arpa/inet.h>
#include <array>
#include <cstdint>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

const uint16_t PORT = 8080;
const size_t BUFFER_SIZE = 1024;

void handle_client(int client_socket) {
  std::array<char, BUFFER_SIZE> buffer{};
  while (true) {
    ssize_t bytes_received = recv(client_socket, buffer.data(), BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      std::cerr << "Client disconnected or error occurred." << std::endl;
      close(client_socket);
      return;
    }
    auto message = std::string(buffer.begin(), buffer.end());
    std::cout << "Received: " << message << std::endl;
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
  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    std::cerr << "Failed to create socket" << std::endl;
    return 1;
  }

  // Listen to loopback address ((127.0.0.1)) only.
  // Listen to all address use INADDR_ANY instead of INADDR_LOOPBACK
  sockaddr_in server_addr{.sin_family = AF_INET,
                          .sin_port = htons(PORT),
                          .sin_addr = {.s_addr = htonl(INADDR_LOOPBACK)}};
  sockaddr *addr = reinterpret_cast<sockaddr *>(&server_addr);

  if (bind(sockfd, addr, sizeof(server_addr)) == -1) {
    std::cerr << "Bind failed, errno " << errno << std::endl;
    return 1;
  }

  if (listen(sockfd, 5) == -1) {
    std::cerr << "Listen failed, errno" << errno << std::endl;
    return 1;
  }

  std::cerr << "Server listening on port " << PORT << "..." << std::endl;

  // Accept the data packet from client and verification
  while (true) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_socket =
        accept(sockfd, reinterpret_cast<sockaddr *>(&client_addr), &client_len);
    if (client_socket == -1) {
      std::cerr << "Accept failed, errno" << errno << std::endl;
      continue;
    }
    // Create thread for each client, to handle them concurrently.
    std::thread(handle_client, client_socket).detach();
    std::cout << "Client connected" << std::endl;
  }

  close(sockfd);
  return 0;
}
