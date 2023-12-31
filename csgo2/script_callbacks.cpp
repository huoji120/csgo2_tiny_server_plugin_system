#include "script_callbacks.h"
#include <functional>
namespace ScriptCallBacks {
std::unordered_map<lua_State*, std::unordered_map<_CallbackNames, int>>
    callbackList;
std::shared_mutex mutex_callbackList;
std::unordered_map<uint32_t, _CallbackNames> callbackNameWithEnumMap{
    {hash_32_fnv1a_const("player_connect"), _CallbackNames::kOnPlayerConnect},
    {hash_32_fnv1a_const("player_disconnect"),
     _CallbackNames::kOnPlayerDisconnect},
    {hash_32_fnv1a_const("player_death"), _CallbackNames::kOnPlayerDeath},
    {hash_32_fnv1a_const("player_chat"), _CallbackNames::kOnPlayerSpeak},
    {hash_32_fnv1a_const("player_spawn"), _CallbackNames::kOnPlayerSpawn},
    {hash_32_fnv1a_const("round_start"), _CallbackNames::kOnRoundStart},
    {hash_32_fnv1a_const("round_end"), _CallbackNames::kOnRoundEnd},
    {hash_32_fnv1a_const("player_hurt"), _CallbackNames::kOnPlayerHurt},
    {hash_32_fnv1a_const("player_team"), _CallbackNames::kOnPlayerTeamChange},
    {hash_32_fnv1a_const("http_request"), _CallbackNames::kOnHttpRequest},
    {hash_32_fnv1a_const("say_text_to_filter"),
     _CallbackNames::kOnSayText2Filter},

};
auto CallBackNameToEnum(const char* name) -> _CallbackNames {
    if (name == nullptr) {
        return _CallbackNames::kError;
    }
    auto hash = hash_32_fnv1a_const(name);
    auto it = callbackNameWithEnumMap.find(hash);
    if (it == callbackNameWithEnumMap.end()) {
        return _CallbackNames::kError;
    }
    return it->second;
}
auto ExcuteCallbackInAllLuaVm(_CallbackNames cbType,
                              std::function<void(lua_State*, int)> cb) -> void {
    std::shared_lock lock(mutex_callbackList);

    for (auto& [pluginName, luaVm] : ScriptEngine::pluginEnvs) {
        // find lua in callbackList
        if (callbackList.find(luaVm) == callbackList.end()) {
            continue;
        }
        const auto luaVMCallbackLists = callbackList[luaVm];
        if (luaVMCallbackLists.find(cbType) == luaVMCallbackLists.end()) {
            continue;
        }
        // LOG("excute callback %d in %s \n", cbType, pluginName.c_str());
        const auto luaRefIndex = luaVMCallbackLists.at(cbType);
        cb(luaVm, luaRefIndex);
    }
}
auto luaCall_onPlayerConnect(int player, int playerSlot, const char* playerName,
                             uint64_t xuid, const char* SteamId,
                             const char* IpAddress, bool isBot) -> void {
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnPlayerConnect,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, player);
                                     lua_pushinteger(luaVm, playerSlot);
                                     lua_pushstring(luaVm, playerName);
                                     lua_pushinteger(luaVm, xuid);
                                     lua_pushstring(luaVm, SteamId);
                                     lua_pushstring(luaVm, IpAddress);
                                     lua_pushboolean(luaVm, isBot);
                                     if (lua_pcall(luaVm, 7, 0, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                 }
                                 // lua_pop(luaVm, 1);
                             });
}
auto luaCall_onPlayerDisconnect(int player, int slot, const char* pszName,
                                uint64_t xuid, const char* pszNetworkID,
                                const char* pszAddress, bool bFakePlayer)
    -> void {
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnPlayerDisconnect,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, player);
                                     lua_pushinteger(luaVm, slot);
                                     lua_pushstring(luaVm, pszName);
                                     lua_pushinteger(luaVm, xuid);
                                     lua_pushstring(luaVm, pszNetworkID);
                                     lua_pushstring(luaVm, pszAddress);
                                     lua_pushboolean(luaVm, bFakePlayer);
                                     if (lua_pcall(luaVm, 7, 0, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                 }
                             });
}
auto luaCall_onPlayerDeath(int victim, int killer, bool isHeadShot) -> void {
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnPlayerDeath,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, victim);
                                     lua_pushinteger(luaVm, killer);
                                     lua_pushboolean(luaVm, isHeadShot);

                                     if (lua_pcall(luaVm, 3, 0, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                 }
                             });
}
auto luaCall_onPlayerSpeak(int speaker, int chatType, std::string message)
    -> bool {
    bool result = false;
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnPlayerSpeak,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, speaker);
                                     lua_pushinteger(luaVm, chatType);
                                     lua_pushstring(luaVm, message.c_str());
                                     if (lua_pcall(luaVm, 3, 1, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                     if (lua_isboolean(luaVm, -1) && result == false) {
                                         result = lua_toboolean(luaVm, -1);
                                     }
                                 }
                             });
    return result;
}
auto luaCall_onPlayerSpawn(int player) -> void {
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnPlayerSpawn,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, player);
                                     if (lua_pcall(luaVm, 1, 0, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                 }
                             });
}
auto luaCall_onRoundStart(int timeLimit) -> void {
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnRoundStart,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, timeLimit);
                                     if (lua_pcall(luaVm, 1, 0, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                 }
                             });
}
auto luaCall_onRoundEnd(int winnerTeam, int reason, const char* message)
    -> void {
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnRoundEnd,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, winnerTeam);
                                     lua_pushinteger(luaVm, reason);
                                     lua_pushstring(luaVm, message);
                                     if (lua_pcall(luaVm, 3, 0, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                 }
                             });
}
auto luaCall_onPlayerHurt(int userid, int attacker, int health, int armor,
                          const char* weapon, int dmg_health, int dmg_armor,
                          int hitgroup) -> void {
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnPlayerHurt,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, userid);
                                     lua_pushinteger(luaVm, attacker);
                                     lua_pushinteger(luaVm, health);
                                     lua_pushinteger(luaVm, armor);
                                     lua_pushstring(luaVm, weapon);
                                     lua_pushinteger(luaVm, dmg_health);
                                     lua_pushinteger(luaVm, dmg_armor);
                                     lua_pushinteger(luaVm, hitgroup);
                                     if (lua_pcall(luaVm, 8, 0, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                 }
                             });
}
auto luaCall_onPlayerTeamChange(int userid, int team, int oldteam,
                                bool disconnect, bool slient, bool isBot)
    -> bool {
    bool result = false;
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnPlayerTeamChange,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, userid);
                                     lua_pushinteger(luaVm, team);
                                     lua_pushinteger(luaVm, oldteam);
                                     lua_pushboolean(luaVm, disconnect);
                                     lua_pushboolean(luaVm, slient);
                                     lua_pushboolean(luaVm, isBot);

                                     if (lua_pcall(luaVm, 6, 1, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                     if (lua_isboolean(luaVm, -1) && result == false) {
                                         result = lua_toboolean(luaVm, -1);
                                     }
                                 }
                             });
    return result;
}
auto luaCall_onHttpRequest(std::string url, std::string metaData,
                           std::string respon, int statusCode) -> void {
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnHttpRequest,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushstring(luaVm, url.c_str());
                                     lua_pushstring(luaVm, metaData.c_str());
                                     lua_pushstring(luaVm, respon.c_str());
                                     lua_pushinteger(luaVm, statusCode);
                                     if (lua_pcall(luaVm, 4, 0, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                 }
                             });
}
auto luCall_onSayText2Filter(int player, uint64_t eMessageType,
                             const char* messeageName, const char* param1,
                             const char* param2, const char* param3,
                             const char* param4) -> bool {
    bool result = false;
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnSayText2Filter,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, player);
                                     lua_pushinteger(luaVm, eMessageType);
                                     lua_pushstring(luaVm, messeageName);
                                     lua_pushstring(luaVm, param1);
                                     lua_pushstring(luaVm, param2);
                                     lua_pushstring(luaVm, param3);
                                     lua_pushstring(luaVm, param4);
                                     if (lua_pcall(luaVm, 7, 1, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                     if (lua_isboolean(luaVm, -1) && result == false) {
                                         result = lua_toboolean(luaVm, -1);
                                     }
                                 }
                             });
    return result;
}
}  // namespace ScriptCallBacks
