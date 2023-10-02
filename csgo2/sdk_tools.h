#pragma once
#include "head.h"
enum _ChatType
{
	kTeam,
	kAll
};
namespace SdkTools {
	auto ProcessChatString(const std::string& input) -> std::tuple<bool, _ChatType, std::string>;
};