#include "script_engine.h"
namespace ScriptEngine {
std::string luaPath;
std::map<std::string, lua_State*> pluginEnvs;
std::shared_mutex mutex_pluginEnvs;

auto callFunction(lua_State* luaVm, const char* funcName) -> int {
    lua_getglobal(luaVm, funcName);
    auto result = 0;
    do {
        if (lua_type(luaVm, -1) == LUA_TNIL) {
            printf("lua_getglobal :%s\n\n", lua_tostring(luaVm, -1));
            result = 0;
            break;
        }
        if (!lua_isfunction(luaVm, -1)) {
            printf("lua_isfunction_err:%s\n\n", lua_tostring(luaVm, -1));
            result = 0;
            break;
        }
        if (lua_pcall(luaVm, 0, 1, 0)) {
            printf("lua_pcall_err:%s\n\n", lua_tostring(luaVm, -1));
            result = 0;
            break;
        }
        const auto result = lua_toboolean(luaVm, 1);
    } while (false);

    lua_pop(luaVm, 1);
    return result;
}

auto initLuaScripts() -> void {
    auto [dirPaths, dirNames] = Tools::GetDirs(luaPath);
    if (dirPaths.size() == 0) {
        LOG("no lua file in %s\n", luaPath.c_str());
        return;
    }
    std::unique_lock lock(mutex_pluginEnvs);
    for (size_t i = 0; i < dirPaths.size(); i++) {
        std::string dirPath = dirPaths[i];
        std::string dirName = dirNames[i];

        lua_State* L = luaL_newstate();

        luaL_openlibs(L);
        ScriptApis::initFunciton(L);

        pluginEnvs[dirName] = L;

        std::string file = dirPath + "\\main.lua";
        if (std::filesystem::exists(file) == false) {
            continue;
        }
        LOG("execute: %s\n", file.c_str());

        if (luaL_dofile(L, file.c_str())) {
            LOG("dofile_err:%s\n\n", lua_tostring(L, -1));
            continue;
        }
        callFunction(L, "Main");
    }
}

auto releaseLuaScripts() -> void {
    std::unique_lock lock(mutex_pluginEnvs);
    for (auto& pair : pluginEnvs) {
        lua_close(pair.second);
    }
    pluginEnvs.clear();
}

auto reloadLuaScripts() -> void {
    releaseLuaScripts();
    initLuaScripts();
}
auto Init() -> void {
    // luaPath = Tools::GetExePath() + "\\huoji_scripts\\";
    luaPath = "F:\\source2\\huoji_scripts\\";
    initLuaScripts();
}
}  // namespace ScriptEngine
