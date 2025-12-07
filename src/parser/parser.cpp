#include "parser/parser.hpp"
#include "executor/builtin.hpp"
#include "executor/program.hpp"
#include "fileutils/executable.hpp"

#include <sstream>
#include <memory>
#include <cstring>
#include <sys/wait.h>

constexpr size_t READ_END = 0;
constexpr size_t WRITE_END = 1;

namespace parser {
  ParsedCommand::ParsedCommand(
    redirect::Redirector& redirector,
    std::unique_ptr<executor::Executor> executor,
    const std::string& command,
    const executor::ArgsList& args
  ) :
    m_redirector{std::move(redirector)}, 
    m_executor{std::move(executor)}, 
    m_command{std::move(command)}, 
    m_args{std::move(args)}
  {};

  const std::string& ParsedCommand::getCommand() {
    return m_command;
  }

  redirect::Redirector& ParsedCommand::getRedirector() {
    return m_redirector;
  }

  void ParsedCommand::exec() {
    if (m_executor) {
      m_executor->exec(m_redirector, m_args);
    } else if (m_command != "") {
      printf("%s: command not found\n", m_command.c_str());
    }
  }

  void ParsedCommand::execAsync() {
    if (m_executor) {
      m_executor->execAsync(m_redirector, m_args);
    } else if (m_command != "") {
      printf("%s: command not found\n", m_command.c_str());
    }
  }

  ParsedCommands::ParsedCommands(std::vector<ParsedCommand>&& commands): m_commands{std::move(commands)} {};
  void ParsedCommands::connect() {
    size_t n = m_commands.size();
    // Do nothing if there is no command
    if (n == 0) return;

    // Setting up pipes
    std::vector<int[2]> pipes(n - 1);
    for (size_t i = 0; i < n - 1; ++i) {
      pipe(pipes[i]);
      m_pipeFds.push_back(pipes[i][READ_END]);
      m_pipeFds.push_back(pipes[i][WRITE_END]);
    }


    for (size_t i = 1; i < n; ++i) {
      auto& prevRedirector = m_commands[i - 1].getRedirector();
      auto& currRedirector = m_commands[i].getRedirector();
      if (prevRedirector.getOutputFd() == -1) {
        prevRedirector.setOutput(
          std::make_unique<redirect::PipeStreamWrapper>(pipes[i - 1][WRITE_END], m_pipeFds, "w")
        );\
      }
      if (currRedirector.getInputFd() == -1) {
        currRedirector.setInput(
          std::make_unique<redirect::PipeStreamWrapper>(pipes[i - 1][READ_END], m_pipeFds, "r")
        );
      }
    }
  };

  void ParsedCommands::Close() {
    for (auto& command: m_commands) command.getRedirector().Close();
    for (int pipeFd: m_pipeFds) close(pipeFd);
  }

  void ParsedCommands::Wait() {
    for (size_t i = 0; i < m_commands.size(); ++i) wait(nullptr);
  }

  void ParsedCommands::exec() {
    connect();
    for (auto& command: m_commands) {
      command.execAsync();
    }
    // Close all created file descriptors
    Close();
    // Wait for sub commands to finish
    Wait();
    m_pipeFds.clear(); // Reset pipeFds;
  }

  constexpr char SPACE = ' ';
  constexpr char BACKSLASH = '\\';
  enum class Quote {
    None = 0,
    Single = '\'',
    Double = '"',
  };
  bool inQuote(Quote quote) {
    return quote != Quote::None;
  }

  struct Token {
    std::string value;
    bool raw;

    bool operator==(const std::string& s) {
      return raw && value == s;
    }

    bool isRedirectOutput() {
      return (*this == ">") || (*this == "1>");
    }

    bool isRedirectError() {
      return *this == "2>";
    }

    bool isAppendOutput() {
      return (*this == ">>") || (*this == "1>>");
    }

    bool isAppendError() {
      return *this == "2>>";
    }

    bool isPipe() {
      return *this == "|";
    }
  };

  std::vector<Token> tokenize(const char* line) {
    std::vector<Token> args;
    std::ostringstream oss;
    Quote quote = Quote::None;
    bool raw = true;
    int i = 0;
    int n = strlen(line);
    while(i < n) {
      char c = line[i];
      switch (c)
      {
      case SPACE:
        if (!inQuote(quote)) {
          if (oss.str() != "") {
            args.emplace_back(oss.str(), raw);
            // Clear oss state
            oss.str("");
            oss.clear();
            // Reset raw state
            raw = true;
          }
        } else {
          oss << c;
        }
        break;
      case BACKSLASH:
        if (quote == Quote::Single) {
          oss << c;
        } else if (quote == Quote::Double) {
          switch (line[i + 1])
          {
          case static_cast<char>(Quote::Double):
          case BACKSLASH:
            oss << line[++i];
            break;
          default:
            oss << c;
            break;
          }
        } else {
          oss << line[++i];
        }
        break;
      case static_cast<char>(Quote::Single):
      case static_cast<char>(Quote::Double):
        raw = false;
        if (!inQuote(quote)) {
          quote = static_cast<Quote>(c);
        } else if (quote == static_cast<Quote>(c)) {
          quote = Quote::None;
        } else {
          oss << c;
        };
        break;    
      default:
        oss << c;
        break;
      }
      ++i;
    } 
    // Append final token (if any)
    if (oss.str() != "") {
      args.emplace_back(oss.str(), raw);
    }
    return args;
  };

  std::unique_ptr<executor::Executor> getExecutor(const std::string& command) {
    auto builtinCommand = executor::getBuiltin(command);
    if (builtinCommand) {
      return std::make_unique<executor::BuiltinExecutor>(builtinCommand.value());
    }
    auto commandPath = fileutils::findExecutable(command);
    if (commandPath) {
      return std::make_unique<executor::ProgramExecutor>(commandPath.value());
    }
    return nullptr;
  }

  ParsedCommands parseTokens(const std::vector<Token>& tokens) {
    size_t n = tokens.size();
    if (n == 0) {
      return ParsedCommands{{}};
    }

    std::vector<ParsedCommand> commands;
    redirect::Redirector redirector{nullptr, nullptr, nullptr};
    executor::ArgsList argsList;
    argsList.reserve(n);
    size_t i = 0;
    while (i < n) {
      auto token = tokens[i];
      if (token.isRedirectOutput()) {
        auto outRedirectFilename = tokens[++i].value;
        redirector.setOutput(std::make_unique<redirect::FileStreamWrapper>(outRedirectFilename, "w"));
      } else if (token.isRedirectError()) {
        auto errRedirectFilename = tokens[++i].value;
        redirector.setError(std::make_unique<redirect::FileStreamWrapper>(errRedirectFilename, "w"));
      } else if (token.isAppendOutput()) {
        auto outAppendFilename = tokens[++i].value;
        redirector.setOutput(std::make_unique<redirect::FileStreamWrapper>(outAppendFilename, "a"));
      } else if (token.isAppendError()) {
        auto errAppendFilename = tokens[++i].value;
        redirector.setError(std::make_unique<redirect::FileStreamWrapper>(errAppendFilename, "a"));
      } else if (token.isPipe()) {
        auto command = argsList[0];
        executor::ArgsList args(
          std::make_move_iterator(argsList.begin() + 1),
          std::make_move_iterator(argsList.end())
        );
        argsList.clear();
        commands.emplace_back(redirector, getExecutor(command), command, args);
      } else {
        argsList.push_back(token.value);
      }
      i++;
    }

    // Add final command
    auto command = argsList[0];
    executor::ArgsList args(
      std::make_move_iterator(argsList.begin() + 1),
      std::make_move_iterator(argsList.end())
    );
    commands.emplace_back(redirector, getExecutor(command), command, args);
    return {std::move(commands)};
  };

  ParsedCommands parseCommandLine(const char* line) {
    auto tokens = tokenize(line);
    return parseTokens(tokens);
  }

}