#include "executor/executor.hpp"
#include <filesystem>

namespace executor {
  void pwdFn(redirect::Redirector& redirector, const ArgsList& args) {
    if (args.size() > 0) {
      fprintf(redirector.getError(), "pwd: too many arguments\n");
      return;
    }
    auto currentPath = std::filesystem::current_path();
    fprintf(redirector.getOutput(), "%s\n", currentPath.c_str());
  }
}