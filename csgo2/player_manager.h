#pragma once
#include "pch.h"
#include "sdk/player/playerslot.h"
namespace ExtendPlayerManager {
auto AddSteamIdToPlayerSteamIdWithNameTable(uint64_t SteamId,
                                            uint64_t PlayerSlot) -> void;
auto GetPlayerSlotBySteamId(uint64_t SteamId) -> uint64_t;
auto SteamIDStringToUInt64(const std::string& steamID) -> uint64_t;
auto RemovePlayerSlotBySteamId(uint64_t SteamId) -> void;
};  // namespace ExtendPlayerManager
