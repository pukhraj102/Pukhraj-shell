#include "parser/parser.hpp"
#include "autocomplete/readline.hpp"
#include "history/history.hpp"

#include <iostream>
#include <cstdlib>

void startup();
void onExit();

int main() {
  startup();  
  while (true)
  {
    auto line = autocomplete::getLine("$ ");
    history::addHistory(line);
    auto parsed = parser::parseCommandLine(line);
    parsed.exec();
    free(line);
  }
  
  return 0;
}

void startup() {
  // Register exit callback
  atexit(onExit);

  // Register autocomplete callback
  autocomplete::registerCallback();

  // Load persistent history
  auto histfile = std::getenv("HISTFILE");
  if (histfile) {
    history::loadHistory(histfile);
  }
}

void onExit() {
  auto histfile = std::getenv("HISTFILE");
  if (histfile) {
    history::appendHistory(histfile);
  }
}
