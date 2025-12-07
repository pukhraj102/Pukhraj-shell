#include "executor/executor.hpp"

#include <filesystem>
#include <cstdio>

namespace executor {
  void cdFn(redirect::Redirector& redirector, const ArgsList& args) {
    if (args.size() != 1) {
      fprintf(redirector.getError(), "usage: cd [path]\n");
      return;
    }
    auto target = args[0];
    if (target[0] == '~') {
      target.replace(0, 1, std::getenv("HOME"));
    }
    if (!std::filesystem::exists(target)) {
      fprintf(redirector.getError(), "cd: %s: No such file or directory\n", target.c_str());
      return;
    }
    std::filesystem::current_path(target);
  }
}