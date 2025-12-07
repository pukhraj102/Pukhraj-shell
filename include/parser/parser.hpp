#pragma once

#include "executor/executor.hpp"

#include <memory>

namespace parser {
  class ParsedCommand {
  private:
    redirect::Redirector m_redirector;
    std::unique_ptr<executor::Executor> m_executor;
    const std::string m_command;
    const executor::ArgsList m_args;
  public:
    ParsedCommand(
      redirect::Redirector& redirector,
      std::unique_ptr<executor::Executor> executor,
      const std::string& command,
      const executor::ArgsList& args
    );
    const std::string& getCommand();
    redirect::Redirector& getRedirector();
    void exec();
    void execAsync();
  };

  class ParsedCommands {
  private:
    std::vector<ParsedCommand> m_commands;
    std::vector<int> m_pipeFds;
    void connect();
    void Close();
    void Wait();
  public:
    ParsedCommands(std::vector<ParsedCommand>&& commands);
    void exec();
  };

  ParsedCommands parseCommandLine(const char* line);
} // namespace parser