#include <arpa/inet.h>
#include <array>
#include <cstdint>
#include <iostream>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

// use fixed port for now
const uint16_t PORT = 8080;
const size_t BUFFER_SIZE = 1024;

int main(int argc, char **argv) {
  if (argc != 1) {
    std::cout << argv[0] << "takes no arguments.\n";
    return 1;
  }

  // Create socket
  // use IPPROTO_TCP instead of 0, because we only care about tcp connection.
  auto sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    std::perror("Failed to create socket, Error");
    return 1;
  }

  // Connecting to loopback address, 127.0.0.1:{port}
  sockaddr_in server_addr{.sin_family = AF_INET,
                          .sin_port = htons(PORT),
                          .sin_addr = {.s_addr = htonl(INADDR_LOOPBACK)}};
  sockaddr *addr = reinterpret_cast<sockaddr *>(&server_addr);
  if (connect(sockfd, addr, sizeof(server_addr)) == -1) {
    std::perror("Connection failed, Error");
    return 1;
  }
  std::cout << "Connected to server" << std::endl;

  std::string message{};
  while (true) {
    std::cout << "Enter message: ";
    std::getline(std::cin, message);
    auto ret = send(sockfd, message.c_str(), message.size(), 0);
    if (ret == -1) {
      std::perror("Send failed, Error");
    }

    std::array<char, BUFFER_SIZE> buffer{};
    ssize_t bytes_received = recv(sockfd, buffer.data(), BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      std::perror("Server disconnected or error occurred. Error");
      break;
    }
    auto response = std::string(buffer.begin(), buffer.end());
    std::cout << "Server response: " << response << std::endl;
  }

  close(sockfd);
  return 0;
}
