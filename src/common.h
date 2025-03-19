#include <unistd.h>
#include <utility>

class FileDescriptor {
private:
  int fd_{-1};

public:
  FileDescriptor(int fd) : fd_(fd) {}
  ~FileDescriptor() {
    if (fd_ != -1) {
      close(fd_);
    }
  }
  // Disable copy
  FileDescriptor(FileDescriptor &) = delete;
  FileDescriptor &operator=(const FileDescriptor &other) = delete;
  // Enable move
  FileDescriptor(FileDescriptor &&other) : fd_(std::exchange(other.fd_, -1)) {}
  FileDescriptor &operator=(FileDescriptor &&) = default;

  operator int() const { return fd_; }
};
