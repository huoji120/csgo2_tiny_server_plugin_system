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
#include "sdk/gameevent/IGameEvent.h"
#include "sdk/tier1/bufferstring.h"
#include "sdk/public/eiface.h"
#include "sdk/player/playerslot.h"

#include "sdk/sdk.h"
#include "sdk/public/mathlib.h"
#include "sdk/public/string_t.h"
#include "sdk/tier1/UtlMemory.hpp"
#include "sdk/tier1/utlfixedmemory.h"
#include "sdk/tier1/utlblockmemory.h"
#include "sdk/tier1/UtlString.hpp"
#include "sdk/interfaces/interfaces.h"
#include "sdk/public/eiface.h"
#include "sdk/gameevent/IGameEvent.h"
#include "sdk/convar/convar.hpp"
#include "sdk/tier1/bufferstring.h"
#include "sdk/public/bitvec.h"
#include "sdk/public/iserver.h"
#include "sdk/public/utlrbtree.h"

#include "sdk/public/utlmap.h"
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
