#pragma once
#include "pch.h"
#include "sdk/gameevent/IGameEvent.h"


typedef bool(__fastcall* FireEventServerSide_t)(CGameEventManager*, IGameEvent*, bool);
typedef void(__fastcall* Host_Say_t)(void*, void*, bool, int, const char*);
typedef bool(__fastcall* OnClientConnect_t)(CPlayerSlot, const char*, uint64_t, const char*, bool unk1, CBufferString*);

extern FireEventServerSide_t original_FireEventServerSide;
extern Host_Say_t original_Host_Say;
namespace hooks {
	auto init() -> bool;
	auto unload() -> void;
}