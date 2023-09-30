// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。
#include <string>
#include <array>
#include <Windows.h>
#include <Psapi.h>
#include <thread>
#include <unordered_map>

#include "framework.h"
#include "stb.hh"

#include "memory.h"
extern void DebugPrintA(const char* format, ...);
//#define LOG DebugPrintA
#define LOG(...) printf(__VA_ARGS__)
#define THE_GAME_SIG(sig) \
    stb::simple_conversion::build<stb::fixed_string{sig}>::value
#include "vmt.h"

#include "./MinHook/include/MinHook.h"
#include "sdk/sdk.h"
#include "interface.h"
#include "module.h"
#include "hash_fnv1a_constexpr.h"
#include "global.h"

#include "offset.h"
#include "events.h"
#include "hooks.h"
#include "VTHook.h"