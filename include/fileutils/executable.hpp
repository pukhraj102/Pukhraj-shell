#pragma once
#include <vector>
#include <string>
#include <optional>

namespace fileutils {
  std::optional<std::string> findExecutable(const std::string& command);
  std::vector<std::string> listExecutables(const std::string& prefix);
}