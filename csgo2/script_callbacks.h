#pragma once
#include "head.h"
namespace ScriptCallBacks {
extern std::shared_mutex mutex_callbackList;
enum class _CallbackNames {
    kError,
    kOnPlayerConnect,
    kOnPlayerDisconnect,
    kOnPlayerDeath
};
extern std::unordered_map<lua_State*, std::unordered_map<_CallbackNames, int>>
    callbackList;
auto CallBackNameToEnum(const char* name) -> ScriptCallBacks::_CallbackNames;
auto luaCall_onPlayerConnect(int player, int playerSlot, const char* playerName,
                             uint64_t xuid, const char* SteamId,
                             const char* IpAddress, bool isBot) -> void;
auto luaCall_onPlayerDisconnect(int player, int slot, const char* pszName,
                                uint64_t xuid, const char* pszNetworkID,
                                const char* pszAddress, bool bFakePlayer)
    -> void;
auto luaCall_onPlayerDeath(int victim, int killer) -> void;
}  // namespace ScriptCallBacks
