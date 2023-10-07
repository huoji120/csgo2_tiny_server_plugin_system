#pragma once
#include "head.h"
namespace ScriptCallBacks {
extern std::shared_mutex mutex_callbackList;
enum class _CallbackNames {
    kError,
    kOnPlayerConnect,
    kOnPlayerDisconnect,
    kOnPlayerDeath,
    kOnPlayerSpeak,
    kOnPlayerSpawn,
    kOnRoundStart,
    kOnRoundEnd,
    kOnPlayerHurt
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
auto luaCall_onPlayerDeath(int victim, int killer, bool isHeadShot) -> void;
auto luaCall_onPlayerSpeak(int speaker, int chatType, std::string message)
    -> bool;
auto luaCall_onPlayerSpawn(int player) -> void;
auto luaCall_onRoundStart(int timeLimit) -> void;
auto luaCall_onRoundEnd(int winnerTeam, int reason, const char* message)
    -> void;
auto luaCall_onPlayerHurt(int userid, int attacker, int health, int armor,
                          const char* weapon, int dmg_health, int dmg_armor,
                          int hitgroup) -> void;
}  // namespace ScriptCallBacks
