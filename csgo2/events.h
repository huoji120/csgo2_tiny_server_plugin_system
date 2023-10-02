#pragma once
#include "head.h"
class CCSPlayerController;
namespace events {
	auto OnPlayerDeathEvent(IGameEvent* event) -> void;
	auto OnPlayerChat(CCSPlayerController* player, std::string message) -> bool;
}
