#include "redirect/redirector.hpp"

namespace redirect {
  Redirector::Redirector(
    std::unique_ptr<StreamWrapper> output,
    std::unique_ptr<StreamWrapper> input,
    std::unique_ptr<StreamWrapper> error
  ): m_output{std::move(output)}, m_input{std::move(input)}, m_error{std::move(error)} {};
  void Redirector::setOutput(std::unique_ptr<StreamWrapper> output) {
    m_output = std::move(output);
  }
  void Redirector::setInput(std::unique_ptr<StreamWrapper> input) {
    m_input = std::move(input);
  }
  void Redirector::setError(std::unique_ptr<StreamWrapper> error) {
    m_error = std::move(error);
  }
  FILE* Redirector::getOutput() {
    return m_output? m_output->getFile(): stdout;
  }
  FILE* Redirector::getInput() {
    return m_input? m_input->getFile(): nullptr;
  }
  FILE* Redirector::getError() {
    return m_error? m_error->getFile(): stderr;
  }
  int Redirector::getOutputFd() {
    return m_output? m_output->getFd(): -1;
  }
  int Redirector::getInputFd() {
    return m_input? m_input->getFd(): -1;
  }
  int Redirector::getErrorFd() {
    return m_error? m_error->getFd(): -1;
  }

  void Redirector::Close() {
    if (m_output) {
      m_output->Close();
      m_output = nullptr;
    }
    
    if (m_input) {
      m_input->Close();
      m_input = nullptr;
    }
    if (m_error) {
      m_error->Close();
      m_error = nullptr;
    }
  }
}