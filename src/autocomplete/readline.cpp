#include "executor/builtin.hpp"
#include "executor/program.hpp"
#include "fileutils/executable.hpp"
#include "history/history.hpp"

#include <readline/readline.h>

namespace autocomplete {
  static char** autocompleteCallback(const char* text, int start, int end);
  static int handleUpKey(int count, int key);
  static int handleDownKey(int count, int key);

  void registerCallback() {
    static bool setup = false;
    if (!setup) {
      rl_attempted_completion_function = autocompleteCallback;
      rl_bind_keyseq("\033[A", handleUpKey);
      rl_bind_keyseq("\033[B", handleDownKey);
      setup = true;
    }
  }

  char* getLine(const char* prompt) {
    return readline(prompt);
  }

  char* autocompleteBuiltin(const char* text, int state) {
    static std::unordered_map<std::string, executor::BuiltinFn>::const_iterator it, end;
    static size_t prefixLen;
    // Initialize static variables
    if (state == 0) {
      auto& builtins = executor::listBuiltins();
      it = builtins.begin();
      end = builtins.end();
      prefixLen = strlen(text);
    };

    while (it != end) {
      auto command = (it++)->first;
      if (command.compare(0, prefixLen, text) == 0) {
        return strdup(command.c_str());
      }
    }
    return nullptr;
  }

  char* autocompleteProgram(const char* text, int state) {
    static std::vector<std::string> programs;
    static size_t prefixLen, idx;

    if (state == 0) {
      programs = fileutils::listExecutables(text);
      idx = 0;
      prefixLen = strlen(text);
    }
    while (idx < programs.size()) {
      auto prog = programs[idx++];
      if (prog.compare(0, prefixLen, text) == 0) {
        return strdup(prog.c_str());
      }
    }
    return nullptr;
  }

  // Get length of a null-terminated char** array
  size_t len(char** a) {
    if (!a) return 0;
    size_t res = 0;
    while (a[res]) {
      res++;
    }
    return res;
  }

  char** concat(char** a, char** b) {
    if (!a && !b) return nullptr;

    auto la = len(a);
    auto lb = len(b);
    char** res = static_cast<char**>(
      malloc(sizeof(char*) * (la + lb + 1)) // Extra 1 for null termination
    );
    for (int i = 0; i < la; ++i) {
      res[i] = a[i];
    }
    for (int i = 0; i < lb; ++i) {
      res[la + i] = b[i];
    }
    res[la + lb] = nullptr;
    // Free a and b to avoid memory leaks
    free(a);
    free(b);
    return res;
  }

  char** autocompleteCallback(const char* text, int start, int end) {
    return concat(
      rl_completion_matches(text, autocompleteBuiltin),
      rl_completion_matches(text, autocompleteProgram)
    );
  }

  int handleUpKey(int count, int key) {
    auto prevCommand = history::getPrevHistory();
    if (prevCommand) {
      auto command = prevCommand.value();
      rl_replace_line(command.c_str(), 1);
      rl_point = command.size();
    }
    rl_redisplay();
    return 0;
  }

  int handleDownKey(int count, int key) {
    auto nextCommand = history::getNextHistory();
    if (nextCommand) {
      auto command = nextCommand.value();
      rl_replace_line(command.c_str(), 1);
      rl_point = command.size();
    }
    rl_redisplay();
    return 0;
  }
}