#include "history/history.hpp"

#include <fstream>
#include <unordered_map>

namespace history {
  static std::vector<std::string> histories;
  static size_t index = 0;
  static std::unordered_map<std::string, size_t> fileIndices;

  const std::vector<std::string>& getHistories() {
    return histories;
  }

  void addHistory(const std::string& command) {
    histories.emplace_back(command);
    index = histories.size();
  }

  std::optional<std::string> getPrevHistory() {
    if (index == 0) return std::nullopt;
    return histories[--index];
  }

  std::optional<std::string> getNextHistory() {
    if (index >= histories.size() - 1) return std::nullopt;
    return histories[++index];
  }

  void loadHistory(const std::string& filename) {
    std::ifstream file(filename, std::ios::in);
    std::string command;
    while (std::getline(file, command)) {
      histories.emplace_back(command);
    }
    index = histories.size();
    fileIndices[filename] = index;
  }

  void saveHistory(const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    for (auto& h: histories) {
      file << h << "\n";
    }
    fileIndices[filename] = histories.size();
  }

  void appendHistory(const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::app);
    auto& idx = fileIndices[filename];
    for (auto i = idx; i < histories.size(); ++i) {
      file << histories[i] << "\n";
    }
    idx = histories.size();
  }
}