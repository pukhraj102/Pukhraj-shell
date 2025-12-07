#pragma once

#include <readline/readline.h>

namespace autocomplete {
  void registerCallback();
  char* getLine(const char* prompt);
}