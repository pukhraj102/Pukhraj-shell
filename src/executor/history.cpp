#include "executor/executor.hpp"
#include "history/history.hpp"

namespace executor {
  void historyFn(redirect::Redirector& redirector, const ArgsList& args) {
    if (args.size() >= 3) {
      fprintf(redirector.getError(), "history: too many arguments\n");
      return;
    }

    // History load/save
    if (args.size() == 2) {
      if (args[0] == "-r") {
        history::loadHistory(args[1]);
      } else if (args[0] == "-w") {
        history::saveHistory(args[1]);
      } else if (args[0] == "-a") {
        history::appendHistory(args[1]);
      } else {
        fprintf(redirector.getError(), "usage: history -[r|w] [filename]\n");
      }
      return;
    }
    
    // Limit
    auto histories = history::getHistories();
    int n = histories.size();
    int limit = n;
    if (args.size() == 1) {
      limit = std::stoi(args[0]);
    }

    for (size_t i = limit; i > 0; --i) {
      fprintf(redirector.getOutput(), "\t%ld  %s\n", n - i + 1, histories[n - i].c_str());
    }
  }
}