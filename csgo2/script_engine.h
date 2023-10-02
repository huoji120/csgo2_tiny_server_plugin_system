#pragma once
#include "head.h"
namespace ScriptEngine {
extern std::map<std::string, lua_State*> pluginEnvs;
extern std::shared_mutex mutex_pluginEnvs;
extern std::shared_mutex mutex_pluginEnvs;
auto Init() -> void;
auto callFunction(const char* funcName) -> int;
auto releaseLuaScripts() -> void;
auto reloadLuaScripts() -> void;
}  // namespace ScriptEngine
