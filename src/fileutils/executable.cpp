#include "fileutils/executable.hpp"

#include <filesystem>
#include <sys/stat.h>
#include <sstream>

namespace fileutils {
#ifdef _WIN32
  constexpr char PATH_SEP = ';';
#else
  constexpr char PATH_SEP = ':';
#endif
  
  static std::vector<std::filesystem::path> listPath();
  static bool isExecutable(const std::filesystem::path& filePath);

  std::optional<std::string> findExecutable(const std::string& command) {
    for (const auto& path: listPath()) {
      auto fullPath = path / command;
      if (isExecutable(fullPath)) {
        return fullPath.string();
      }
    }
    return std::nullopt;
  }

  std::vector<std::filesystem::path> listPath() {
    const char* pathEnv = std::getenv("PATH");
    if (!pathEnv) return {};

    std::vector<std::filesystem::path> dirs;
    std::stringstream ss(pathEnv);
    std::string p;
    while(std::getline(ss, p, PATH_SEP)) {
      dirs.push_back(std::move(p));
    }
    return dirs;
  }

  std::vector<std::string> listExecutables(const std::string& prefix) {
    std::vector<std::string> res;
    auto prefixLen = prefix.size();
    std::error_code ec;
    for (const auto& path: listPath()) {
      for (auto &entry: std::filesystem::directory_iterator(path, ec)) {
        if (ec) break;
        auto fullPath = entry.path();
        auto programName = fullPath.filename().string();
        if (isExecutable(fullPath) && programName.compare(0, prefixLen, prefix) == 0) {
          res.push_back(std::move(programName));
        }
      }
    }
    return res;
  };

#ifdef _WIN32
  #include <windows.h>
  bool isExecutable(const std::filesystem::path& p) {
      auto ext = p.extension().string();
      return ext == ".exe" || ext == ".bat" || ext == ".cmd" || ext == ".com";
  }
#else
  #include <sys/stat.h>
  bool isExecutable(const std::filesystem::path& p) {
      struct stat st {};
      if (stat(p.c_str(), &st) != 0) return false;
      return (st.st_mode & S_IFREG) && (st.st_mode & S_IXUSR);
  }
#endif
}