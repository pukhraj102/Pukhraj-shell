#include "executor/executor.hpp"

namespace executor {
  void echoFn(redirect::Redirector& redirector, const ArgsList& args) {
    if (!args.empty()) {
      fprintf(redirector.getOutput(), "%s", args[0].c_str());
    }
    for (auto i = 1; i < args.size(); ++i) {
      fprintf(redirector.getOutput(), " %s", args[i].c_str());
    }
    fprintf(redirector.getOutput(), "\n");
  }
}