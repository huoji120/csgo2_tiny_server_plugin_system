#include "script_apis.h"
namespace ScriptApis {
// 返回是返回值数量,返回值内容要自己push到stack上
auto luaApi_ListenToGameEvent(lua_State* luaVm) -> int {
    const auto eventName = lua_tostring(luaVm, 1);
    do {
        if (eventName == nullptr) {
            LOG("luaApi_ListenToGameEvent eventName == nullptr || callbackName "
                "== "
                "nullptr\n");
            break;
        }
        if (!lua_isfunction(luaVm, 2)) {
            LOG("luaApi_ListenToGameEvent callback is not a function\n");
            break;
        }
        auto callbackType = ScriptCallBacks::CallBackNameToEnum(eventName);
        if (callbackType == ScriptCallBacks::_CallbackNames::kError) {
            LOG("luaApi_ListenToGameEvent unknown event name: %s\n", eventName);
            break;
        }
        // 将回调添加到映射中
        std::unique_lock lock(ScriptCallBacks::mutex_callbackList);
        ScriptCallBacks::callbackList[luaVm][callbackType] = luaL_ref(luaVm, LUA_REGISTRYINDEX);

        LOG("luaApi_ListenToGameEvent eventName:%s callback added\n",
            eventName);
    } while (false);

    lua_pop(luaVm, 2);  // 清理堆栈
    return 0;
}
auto initFunciton(lua_State* luaVm) -> void {
    lua_register(luaVm, "ListenToGameEvent", luaApi_ListenToGameEvent);
}
};  // namespace ScriptApis
