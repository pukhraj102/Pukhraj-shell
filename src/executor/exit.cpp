#include "executor/executor.hpp"

namespace executor {
  void exitFn(redirect::Redirector& redirector, const ArgsList& args) {
    if (args.size() >= 2) {
      fprintf(redirector.getError(), "exit: too many arguments\n");
      return;
    }

    int exitCode = 0;
    if (args.size() == 1) {
      exitCode = std::stoi(args[0]);
    }
    exit(exitCode);
  } 
}