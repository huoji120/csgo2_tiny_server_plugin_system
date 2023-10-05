#pragma once
#include "head.h"

typedef bool(__fastcall* FireEventServerSide_t)(CGameEventManager*, IGameEvent*, bool);
typedef void(__fastcall* Host_Say_t)(void*, void*, bool, int, const char*);
typedef void(__fastcall* OnClientConnect_t)(void*, CPlayerSlot, const char*, uint64_t, const char*, const char*, bool);
typedef void(__fastcall* OnClientDisconnect_t)(void*, CPlayerSlot, int, const char*, uint64_t, const char*);
typedef void(__fastcall* StartupServer_t)(void*, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*);
typedef void(__fastcall* GameFrame_t)(void*, bool, bool, bool);
namespace hooks {
	extern Host_Say_t original_Host_Say;
	auto init() -> bool;
	auto unload() -> void;
}