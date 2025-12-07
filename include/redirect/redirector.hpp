#pragma once

#include "redirect/stream.hpp"

namespace redirect {

  class Redirector {
  private:
    std::unique_ptr<StreamWrapper> m_output;
    std::unique_ptr<StreamWrapper> m_input;
    std::unique_ptr<StreamWrapper> m_error;
  public:
    Redirector(
      std::unique_ptr<StreamWrapper> output = nullptr,
      std::unique_ptr<StreamWrapper> input = nullptr,
      std::unique_ptr<StreamWrapper> error = nullptr
    );
    void Close();
    void setOutput(std::unique_ptr<StreamWrapper> output);
    void setError(std::unique_ptr<StreamWrapper> error);
    void setInput(std::unique_ptr<StreamWrapper> input);
    FILE* getOutput();
    FILE* getInput();
    FILE* getError();
    int getOutputFd();
    int getInputFd();
    int getErrorFd();
  };

}