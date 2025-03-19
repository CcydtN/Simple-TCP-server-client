#include <iostream>

int main(int argc, char **argv) {
  if (argc != 1) {
    std::cout << argv[0] << "takes no arguments.\n";
    return 1;
  }
  std::cout << "This is server" << ".\n";
  return 0;
}
