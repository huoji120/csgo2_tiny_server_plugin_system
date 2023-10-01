#pragma once
#include "pch.h"
#include "sdk/gameevent/IGameEvent.h"
#include "sdk/tier1/bufferstring.h"

typedef bool(__fastcall* FireEventServerSide_t)(CGameEventManager*, IGameEvent*, bool);
typedef void(__fastcall* Host_Say_t)(void*, void*, bool, int, const char*);
typedef void(__fastcall* OnClientConnect_t)(void*, CPlayerSlot, const char*, uint64_t, const char*, const char*, bool);
typedef void(__fastcall* OnClientDisconnect_t)(void*, CPlayerSlot, int, const char*, uint64_t, const char*);

extern FireEventServerSide_t original_FireEventServerSide;
extern Host_Say_t original_Host_Say;
namespace hooks {
	auto init() -> bool;
	auto unload() -> void;
}