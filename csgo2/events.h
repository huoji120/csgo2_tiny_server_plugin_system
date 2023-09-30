#pragma once
#include "pch.h"
#include "sdk/gameevent/IGameEvent.h"

namespace events {
	auto OnPlayerDeathEvent(IGameEvent* event) -> void;
	auto OnPlayerChat(IGameEvent* event) -> void;
}
