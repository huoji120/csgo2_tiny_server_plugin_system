#pragma once
#include "head.h"
inline int PlayerSlot_to_EntityIndex(int PlayerSlot) { return PlayerSlot + 1; }
inline int EntityIndex_to_PlayerSlot(int EntityIndex) { return EntityIndex - 1; }
enum _ChatType
{
	kTeam,
	kAll
};
namespace SdkTools {
	auto ProcessChatString(const std::string& input) -> std::tuple<bool, _ChatType, std::string>;
};