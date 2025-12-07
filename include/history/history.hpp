#pragma once

#include <vector>
#include <string>
#include <optional>

namespace history {
  const std::vector<std::string>& getHistories();
  void addHistory(const std::string& command);
  std::optional<std::string> getPrevHistory();
  std::optional<std::string> getNextHistory();

  void loadHistory(const std::string& filename);
  void saveHistory(const std::string& filename);
  void appendHistory(const std::string& filename);
}