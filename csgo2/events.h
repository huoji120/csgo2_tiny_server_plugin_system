#pragma once
#include "head.h"
class CCSPlayerController;
namespace events {
auto OnPlayerDeathEvent(IGameEvent* event) -> void;
auto OnPlayerChat(CCSPlayerController* player, std::string message) -> bool;
auto OnPlayerConnect(int slot, const char* pszName, uint64_t xuid,
                     const char* pszNetworkID, const char* pszAddress,
                     bool bFakePlayer) -> void;
auto OnPlayerDisconnect(int slot, const char* pszName, uint64_t xuid,
                        const char* pszNetworkID, const char* pszAddress,
                        bool bFakePlayer) -> void;
}  // namespace events
