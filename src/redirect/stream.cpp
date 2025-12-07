#include "redirect/stream.hpp"

#include <utility>
#include <fcntl.h>

namespace redirect {
  static bool is_fd_open(int fd) {
    return fcntl(fd, F_GETFL) != -1;
  }

  FILE* StreamWrapper::getFile() {
    return m_file;
  }
  int StreamWrapper::getFd() {
    return m_file? fileno(m_file): -1;
  }
  StreamWrapper::StreamWrapper(FILE* file): m_file{file} {};

  FileStreamWrapper::FileStreamWrapper(const std::string& filename, const char* mode)
  : StreamWrapper{fopen(filename.c_str(), mode)} {};
  FileStreamWrapper::FileStreamWrapper(FileStreamWrapper&& other)
  : StreamWrapper{std::exchange(other.m_file, nullptr)} {};
  FileStreamWrapper& FileStreamWrapper::operator=(FileStreamWrapper&& other) {
    Close();
    m_file = std::exchange(other.m_file, nullptr);
    return *this;
  }
  FileStreamWrapper::~FileStreamWrapper() {
    Close();
  }
  void FileStreamWrapper::Close() {
    if (m_file) {
      fclose(m_file);
      m_file = nullptr;
    }
  }
  
  PipeStreamWrapper::PipeStreamWrapper(int fd, const std::vector<int>& otherFds, const char* mode)
    : StreamWrapper{fdopen(fd, mode)}, 
      m_otherFds{otherFds} 
  {}
  PipeStreamWrapper::PipeStreamWrapper(PipeStreamWrapper&& other)
    : StreamWrapper{std::exchange(other.m_file, nullptr)},
      m_otherFds{std::move(other.m_otherFds)}
  {}
  PipeStreamWrapper& PipeStreamWrapper::operator=(PipeStreamWrapper&& other) {
    Close();
    m_file = std::exchange(other.m_file, nullptr);
    m_otherFds = std::move(other.m_otherFds);
    return *this;
  }
  PipeStreamWrapper::~PipeStreamWrapper() {
    Close();
  }
  void PipeStreamWrapper::Close() {
    if (m_file) {
      fclose(m_file);
      for (int otherFd: m_otherFds) {
        if (is_fd_open(otherFd)) {
          close(otherFd);
        }
      }
      m_file = nullptr;
    }
  }

}