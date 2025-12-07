#include "executor/builtin.hpp"
#include "fileutils/executable.hpp"

namespace executor {
  void typeFn(redirect::Redirector& redirector, const ArgsList& args) {
    if (args.size() != 1) {
      fprintf(redirector.getError(), "usage: type [command]\n");
      return;
    }

    std::string command = args[0];
    if (getBuiltin(command)) {
      fprintf(redirector.getOutput(), "%s is a shell builtin\n", command.c_str());
      return;
    }
    auto fullCommandPath = fileutils::findExecutable(command);
    if (fullCommandPath) {
      fprintf(redirector.getOutput(), "%s is %s\n", command.c_str(), fullCommandPath.value().c_str());
      return;
    }
    fprintf(redirector.getOutput(), "%s: not found\n", command.c_str());
  }
}