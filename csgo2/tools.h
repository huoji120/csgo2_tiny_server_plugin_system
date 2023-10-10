#pragma once
#include "head.h"
namespace Tools {
auto GetExeFileName() -> std::string;
auto GetExePath() -> std::string;
auto GetFiles(const std::string& path, std::vector<std::string>& files) -> void;
auto GetDirs(const std::string& path)
    -> std::pair<std::vector<std::string>, std::vector<std::string>>;
auto toLower(const std::string& str) -> std::string;
};  // namespace Tools
