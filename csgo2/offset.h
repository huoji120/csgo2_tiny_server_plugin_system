#pragma once
#include "head.h"
#define SERVER_HASH_FUCNTION_KEY 0x31415926
typedef uint64_t(__fastcall* HashFunction_t)(const char*, unsigned int, unsigned int);

class CSchemaSystem;
class CGameResourceService;
namespace Offset {
namespace InterFaces {
    extern CSchemaSystem* SchemaSystem;
    extern IGameEventManager2* GameEventManager;
    extern CGameEventManager* CGameEventManger;
    extern CGameResourceService* GameResourceServiceServer;
    extern IServerGameClients* IServerGameClient;
    extern IVEngineServer2* IVEngineServer;
};
static const auto pattern_CGameEventManager = THE_GAME_SIG("48 ?? ?? ?? ?? ?? ?? 48 89 ?? ?? ?? 48 89 01 48 8B D9 48 ?? ?? ?? ?? ?? ?? 48 89 ?? ?? E8 ?? ?? ?? ?? 48 ?? ?? ?? ?? ?? ??");
static const auto pattern_NetworkStateChanged = THE_GAME_SIG("4C 8B C9 48 8B 09 48 85 C9 74 ? 48 8B 41 10");
static const auto pattern_FireEventServerSide = THE_GAME_SIG(
    "40 53 57 41 54 41 55 41 56 48 ?? ?? ?? 4C 8B F2 4C 8B E1 BA ?? ?? ?? "
    "?? 48 ?? ?? ?? ?? ?? ?? 45 0F B6 E8 E8 ?? ?? ?? ?? 48 85 C0 75 ?? 48 "
    "?? ?? ?? ?? ?? ?? 48 8B ?? ?? 80 ?? ?? 74 ?? 49 8B 06 49 8B CE FF ?? "
    "?? 48 8B D0 48 ?? ?? ?? ?? ?? ?? FF ?? ?? ?? ?? ??");
static const auto pattern_fnGetBaseEntity =
    THE_GAME_SIG("8B D3 E8 ? ? ? ? 48 8B F8 48 85 C0 74 76");
static const auto pattern_fnGetHighestEntityIndex =
    THE_GAME_SIG("33 DB E8 ? ? ? ? 8B 08");
static const auto pattern_fnGetLocalPlayerController =
    THE_GAME_SIG("E8 ? ? ? ? 49 89 47 08");
//"\"Console<0>\" say \"%s\"\n"
static const auto pattern_fnHost_SayPtr =
    THE_GAME_SIG("44 89 4C 24 ?? 44 88 44 24 ?? 55 53 56 57 41 54 41 55");
static const auto pattern_ServerHashFunctionPtr =
    THE_GAME_SIG("48 89 ?? ?? ?? 57 48 ?? ?? ?? ?? ?? ?? 33 C0 8B DA 41 8B F8 48 89 ?? ?? ?? 4C 8B C1 C7 44 ?? ?? ?? ?? ?? ?? 44 8B CA 89 44 ?? ?? 48 8D ?? ?? ?? 88 44 ?? ?? 33 D2");
static const auto pattern_MaxPlayerNumsPtr = 
    THE_GAME_SIG("41 3B 87 ?? ?? ?? ?? 0F 8E ?? ?? ?? ?? 8B 0D ?? ?? ?? ??");
extern uint64_t GameResourceServicePtr;
extern uint64_t FireEventServerSidePtr;
extern uint64_t Module_tier0;
extern uint64_t NetworkStateChangedPtr;
extern uint64_t Host_SayPtr;
extern uint64_t MaxPlayerNumsPtr;
extern HashFunction_t FnServerHashFunction;
auto Init() -> bool;
};  // namespace Offset
