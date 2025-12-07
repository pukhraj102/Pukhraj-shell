#include "executor/program.hpp"

#include <filesystem>

namespace executor {
  ProgramExecutor::ProgramExecutor(const std::string& fullPath): m_fullPath(fullPath) {};
#ifdef _WIN32
#include <windows.h>

  void ProgramExecutor::exec(redirect::Redirector& redirector, const ArgsList& args) const {
    std::string cmd = "\"" + m_fullPath + "\"";
    for (auto& a : args) {
      cmd += " \"" + a + "\"";
    }

    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};

    CreateProcess(
      exe.c_str(),
      cmd.data(),
      NULL, NULL, FALSE, 0, NULL, NULL,
      &si, &pi
    );

    WaitForSingleObject(pi.hProcess, INFINITE);
}
#else
#include <unistd.h>
#include <sys/wait.h>

  void ProgramExecutor::execAsync(redirect::Redirector& redirector, const ArgsList& args) const {
    auto pid = fork();
    if (pid == 0) {
      // Piping
      if (redirector.getOutputFd() != -1) {
        dup2(redirector.getOutputFd(), STDOUT_FILENO);
      }
      if (redirector.getInputFd() != -1) {
        dup2(redirector.getInputFd(), STDIN_FILENO);
      }
      if (redirector.getErrorFd() != -1) {
        dup2(redirector.getErrorFd(), STDERR_FILENO);
      }
      redirector.Close();

      std::vector<char*> argv;
      argv.push_back(const_cast<char*>(getName().c_str())); // Add program name as first argument
      for (const auto& arg: args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
      }
      argv.push_back(nullptr);
      int status = execve(m_fullPath.c_str(), argv.data(), environ);
      exit(status);
    }
  }

  void ProgramExecutor::exec(redirect::Redirector& redirector, const ArgsList& args) const {
    execAsync(redirector, args);
    redirector.Close();
    wait(nullptr);
  }
#endif

  std::string ProgramExecutor::getName() const {
    auto path = std::filesystem::path(m_fullPath);
    return path.filename();
  }
}