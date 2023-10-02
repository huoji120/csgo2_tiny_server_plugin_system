#pragma once
#include "head.h"
namespace Tools {
	auto GetExeFileName() -> std::string;
	auto GetExePath() -> std::string;
	auto GetFiles(const std::string& path, std::vector<std::string>& files) -> void;
};