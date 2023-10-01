#pragma once
#include "pch.h"
#include "sdk/player/playerslot.h"
namespace PlayerManager {
auto AddPlayerNameToPlayerNameList(const CPlayerSlot PlayerSlot,
                                   const char* PlayerName) -> void;
auto RemovePlayerNameFromPlayerNameList(const CPlayerSlot PlayerSlot,
                                        const char* PlayerName) -> void;
auto GetPlayerNameByPlayerSlot(const CPlayerSlot PlayerSlot) -> std::string;
};  // namespace PlayerManager
