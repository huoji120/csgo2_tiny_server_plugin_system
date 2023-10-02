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
    {hash_32_fnv1a_const("player_death"), _CallbackNames::kOnPlayerDeath}};
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
        LOG("excute callback %d in %s \n", cbType, pluginName.c_str());
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
auto luaCall_onPlayerDeath(int victim, int killer) -> void {
    ExcuteCallbackInAllLuaVm(_CallbackNames::kOnPlayerDeath,
                             [&](lua_State* luaVm, int refIndex) -> void {
                                 lua_rawgeti(luaVm, LUA_REGISTRYINDEX,
                                             refIndex);
                                 if (lua_isfunction(luaVm, -1)) {
                                     lua_pushinteger(luaVm, victim);
                                     lua_pushinteger(luaVm, killer);
                                     if (lua_pcall(luaVm, 2, 0, 0) != LUA_OK) {
                                         LOG("Error calling Lua callback: %s\n",
                                             lua_tostring(luaVm, -1));
                                         lua_pop(luaVm, 1);
                                     }
                                 }
                             });
}
}  // namespace ScriptCallBacks
