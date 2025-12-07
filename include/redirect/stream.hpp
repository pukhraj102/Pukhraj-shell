#pragma once

#include <memory>
#include <string>
#include <cstdio>
#include <vector>

namespace redirect {
  class StreamWrapper {
  public:
    FILE* getFile();
    int getFd();

    virtual void Close() = 0;
    virtual ~StreamWrapper() = default;
    
  protected:
    StreamWrapper(FILE* file);
    FILE* m_file;
  };

  class FileStreamWrapper: public StreamWrapper {
  public:
    FileStreamWrapper(const std::string& filename, const char* mode);
    FileStreamWrapper(const FileStreamWrapper& other) = delete;
    FileStreamWrapper& operator=(const FileStreamWrapper& other) = delete;
    FileStreamWrapper(FileStreamWrapper&& other);
    FileStreamWrapper& operator=(FileStreamWrapper&& other);
    ~FileStreamWrapper() override;
    void Close() override;
  };

  class PipeStreamWrapper: public StreamWrapper {
  public:
    PipeStreamWrapper(int fd, const std::vector<int>& otherFds, const char* mode);
    PipeStreamWrapper(const PipeStreamWrapper& other) = delete;
    PipeStreamWrapper& operator=(const PipeStreamWrapper& other) = delete;
    PipeStreamWrapper(PipeStreamWrapper&& other);
    PipeStreamWrapper& operator=(PipeStreamWrapper&& other);
    ~PipeStreamWrapper() override;
    void Close() override;
  private:
    std::vector<int> m_otherFds;
  };
}