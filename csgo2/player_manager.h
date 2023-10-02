#pragma once
#include "head.h"
namespace ExtendPlayerManager {
auto AddSteamIdToPlayerSteamIdWithNameTable(uint64_t SteamId,
                                            uint64_t PlayerSlot) -> void;
auto GetPlayerSlotBySteamId(uint64_t SteamId) -> uint64_t;
auto SteamIDStringToUInt64(const std::string& steamID) -> uint64_t;
auto RemovePlayerSlotBySteamId(uint64_t SteamId) -> void;
auto GetPlayerByPlayerSlot(uint64_t playerSlot) -> CCSPlayerController*;
};  // namespace ExtendPlayerManager
