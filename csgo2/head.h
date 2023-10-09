#pragma once
#include <string>
#include <array>
#include <Windows.h>
#include <Psapi.h>
#include <thread>
#include <unordered_map>
#include <map>
#include <shared_mutex>
#include <sstream>
#include <filesystem>

#include "framework.h"
#include "stb.hh"
// #define LOG DebugPrintA
static void DebugPrintA(const char* format, ...) {
    std::string temp;
    va_list marker = {0};
    va_start(marker, format);
    size_t num_of_chars = _vscprintf(format, marker);
    if (num_of_chars > temp.capacity()) {
        temp.reserve(num_of_chars + 1);
    }
    vsprintf_s(const_cast<char*>(temp.c_str()), num_of_chars + 1, format,
               marker);
    OutputDebugStringA(temp.c_str());
}
#define LOG(...) printf(__VA_ARGS__)
#define THE_GAME_SIG(sig) \
    stb::simple_conversion::build<stb::fixed_string{sig}>::value
// 第三方库
#include "./MinHook/include/MinHook.h"
#include "hash_fnv1a_constexpr.h"
// 工具库
#include "vector.h"
#include "vmt.h"
#include "memory.h"

// sdk

#include "sdk/sdk.h"

#include "offset.h"
#include "native_sdk.h"

#include "interface.h"
#include "module.h"
#include "global.h"

#include "events.h"
#include "hooks.h"
#include "vmthook.h"

#include "player_manager.h"
#include "sdk_tools.h"

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "tools.h"
#include "script_engine.h"
#include "script_apis.h"
#include "script_callbacks.h"
#include "timer.h"
#include "weapon.h"
#include "lua-cjson/lua_cjson.h"
#include "Server.h"