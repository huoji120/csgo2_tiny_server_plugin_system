#include "script_engine.h"
namespace ScriptEngine {
lua_State* luaVm;
std::string luaPath;
auto callFunction(const char* funcName) -> int {
    lua_getglobal(luaVm, funcName);
    _ASSERT(lua_isfunction(luaVm,  -1));
    if (lua_pcall(luaVm, 0, 1, 0)) {
        printf("lua_pcall_err:%s\n\n", lua_tostring(luaVm, -1));
    }
    const auto result = lua_toboolean(luaVm, 1);
    lua_pop(luaVm, 1);
    return result;
}
auto initFunciton() -> void {
}
auto initLuaScripts() -> void {
    std::vector<std::string> files;
    Tools::GetFiles(luaPath, files);
    if (files.size() == 0) {
        LOG("no lua file in %s\n", luaPath.c_str());
        exit(0);
    }
    for (size_t i = 0; i < files.size(); i++) {
        std::string file = files[i];
        if (file.find(".lua") != std::string::npos) {
            LOG("excute: %s\n", file.c_str());

            if (luaL_dofile(luaVm, file.c_str())) {
                LOG("dofile_err:%s\n\n", lua_tostring(luaVm, -1));
                continue;
            }
            callFunction("main");
        }
    }

}
auto Init() -> void {
    luaVm = luaL_newstate();
    luaPath = Tools::GetExePath() + "\\huoji_scripts\\";
    initFunciton();
    initLuaScripts();
}
}
