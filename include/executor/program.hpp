#pragma once

#include "executor/executor.hpp"

namespace executor {
  class ProgramExecutor: public Executor
  {
  private:
    const std::string m_fullPath;
  public:
    ProgramExecutor(const std::string& fullPath);
    void exec(redirect::Redirector& redirector, const ArgsList& args) const override;
    void execAsync(redirect::Redirector& redirector, const ArgsList& args) const override;
    std::string getName() const;
  };
}