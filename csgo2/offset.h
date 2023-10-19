#pragma once
#include "head.h"
#define SERVER_HASH_FUCNTION_KEY 0x31415926
class CEntityInstance;
class CCSPlayerPawn;
class CGameEntitySystem;
class CCSPlayerController;
class CBaseEntity;
typedef uint64_t(__fastcall* HashFunction_t)(const char*, unsigned int,
                                             unsigned int);
typedef void(__fastcall* StateChanged_t)(void* networkTransmitComponent,
                                         CEntityInstance* ent, uint64_t offset,
                                         int a4, int a5);
typedef void(__fastcall* NetworkStateChanged_t)(uintptr_t chainEntity,
                                                uintptr_t offset, uintptr_t a3);
typedef void*(__fastcall* CreateGameRuleInterFace_t)();
typedef bool(__fastcall* RespawnPlayerInDeathMatch_t)(CCSPlayerPawn* player);
typedef void(__fastcall* GiveNamedItem_t)(void* itemService,
                                          const char* pchName, void* iSubType,
                                          void* pScriptItem, void* a5,
                                          void* a6);
typedef void*(__fastcall* EntityRemove_t)(CGameEntitySystem*, void*, void*,
                                          uint64_t);
typedef void*(__fastcall* UTIL_SayTextFilter_t)(IRecipientFilter&, const char*,
                                                CCSPlayerController*, uint64_t);
typedef void(__fastcall* UTIL_ClientPrintAll_t)(
    int msg_dest, const char* msg_name, const char* param1, const char* param2,
    const char* param3, const char* param4);
typedef void(__fastcall* ClientPrint_t)(CCSPlayerController* player,
                                        int msg_dest, const char* msg_name,
                                        const char* param1, const char* param2,
                                        const char* param3, const char* param4);
typedef void(__fastcall* CCSWeaponBase_Spawn_t)(CBaseEntity*, void*);
typedef void(__fastcall* PlayerChangeName_t)(CBaseEntity*, char*);
class CSchemaSystem;
class CGameResourceService;
class CLocalize;
class CCSGameRules;
namespace Offset {
namespace InterFaces {
extern CSchemaSystem* SchemaSystem;
extern IGameEventManager2* GameEventManager;
extern CGameEventManager* CGameEventManger;
extern CGameResourceService* GameResourceServiceServer;
extern IServerGameClients* IServerGameClient;
extern IVEngineServer2* IVEngineServer;
extern CLocalize* ILocalize;
extern INetworkServerService* INetworkServerServiceInteFace;
extern ISource2Server* ISource2ServerInterFace;
extern CCSGameRules* CCSGameRulesInterFace;
extern ICvar* IVEngineCvar;
};  // namespace InterFaces
static const auto pattern_FnUTIL_SayText2FilterPtr = THE_GAME_SIG(
    "48 89 5C 24 ?? 55 56 57 48 8D 6C 24 ?? 48 81 EC ?? ?? ?? ?? 41 0F B6 F8");
static const auto pattern_PlayerChangeName = THE_GAME_SIG(
    "4C 8B DC 55 56 41 54 49 8D ?? ?? ?? ?? ?? 48 ?? ?? ?? ?? ?? ?? 48 8B F1 "
    "41 ?? ?? ?? ?? ?? 8B 89 ?? ?? ?? ?? 4C 8B D2 41 3B CC 0F ?? ?? ?? ?? ?? "
    "4C ?? ?? ?? ?? ?? ?? 4D 85 C0 0F ?? ?? ?? ?? ?? 83 ?? ?? 0F ?? ?? ?? ?? "
    "?? 8B C1 25 ?? ?? ?? ??");
static const auto pattern_VscriptPath =
    THE_GAME_SIG("BE 01 ?? ?? ?? 2B D6 74 ?? 3B D6");
static const auto pattern_CGameEventManager = THE_GAME_SIG(
    "48 ?? ?? ?? ?? ?? ?? 48 89 ?? ?? ?? 48 89 01 48 8B D9 48 ?? ?? ?? ?? ?? "
    "?? 48 89 ?? ?? E8 ?? ?? ?? ?? 48 ?? ?? ?? ?? ?? ??");
static const auto pattern_NetworkStateChanged =
    THE_GAME_SIG("4C 8B C9 48 8B 09 48 85 C9 74 ? 48 8B 41 10");
static const auto pattern_FnStateChangedPtr =
    THE_GAME_SIG("48 89 54 24 10 55 53 57 41 55");
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
static const auto pattern_ServerHashFunctionPtr = THE_GAME_SIG(
    "48 89 ?? ?? ?? 57 48 ?? ?? ?? ?? ?? ?? 33 C0 8B DA 41 8B F8 48 89 ?? ?? "
    "?? 4C 8B C1 C7 44 ?? ?? ?? ?? ?? ?? 44 8B CA 89 44 ?? ?? 48 8D ?? ?? ?? "
    "88 44 ?? ?? 33 D2");
static const auto pattern_MaxPlayerNumsPtr =
    THE_GAME_SIG("41 3B 87 ?? ?? ?? ?? 0F 8E ?? ?? ?? ?? 8B 0D ?? ?? ?? ??");
static const auto pattern_FnGiveNamedItemPtr = THE_GAME_SIG(
    "48 89 5C 24 18 48 89 74 24 20 55 57 41 54 41 56 41 57 48 8D 6C 24 D9");
static const auto pattern_CreateCCSGameRulesInterFacePtr = THE_GAME_SIG(
    "48 ?? ?? ?? ?? ?? ?? 48 8B 01 FF ?? ?? ?? ?? ?? 48 8D ?? ?? ?? E8 ?? ?? "
    "?? ?? 4C 8D ?? ?? ?? 49 8B ?? ?? 49 8B ?? ?? 49 8B ?? ?? 49 8B E3 41 5F "
    "41 5E 5F C3");
static const auto pattern_FnRespawnPlayer = THE_GAME_SIG(
    "48 89 ?? ?? ?? 48 89 ?? ?? ?? 56 48 ?? ?? ?? ?? ?? ?? 48 8B DA 48 8B E9 "
    "48 85 D2 0F ?? ?? ?? ?? ?? 48 8B 02 48 8B CA FF ?? ?? ?? ?? ?? 84 C0 0F "
    "?? ?? ?? ?? ?? 83 BB ?? ?? ?? ?? ?? 0F ?? ?? ?? ?? ??");
static const auto pattern_FnRespawnPlayerInDeathMatch = THE_GAME_SIG(
    "48 89 ?? ?? ?? 57 48 ?? ?? ?? 48 8D ?? ?? ?? 48 8B F9 E8 ?? ?? ?? ?? 83 "
    "?? ?? 74 ?? 48 ?? ?? ?? ?? ?? ?? 48 8B CF 48 8B 10 48 8B ?? ?? ?? ?? ?? "
    "48 8D ?? ?? ?? E8 ?? ?? ?? ?? 48 ?? ?? ?? ?? ?? ??");
static const auto pattern_FnEntityRemove = THE_GAME_SIG(
    "48 85 D2 0F ?? ?? ?? ?? ?? 57 48 ?? ?? ?? 48 89 ?? ?? ?? 48 8B F9 48 8B "
    "?? ?? 48 85 DB 0F ?? ?? ?? ?? ?? 48 ?? ?? ?? 75 ?? 33 D2 48 8B CB E8 ?? "
    "?? ?? ?? 48 8D ?? ?? 41 ?? ?? 48 8B D3 48 8B ?? ?? ?? 48 ?? ?? ?? 5F E9 "
    "?? ?? ?? ??");
static const auto pattern_FnUTIL_SayTextFilter = THE_GAME_SIG(
    "48 89 5C 24 ?? 55 56 57 48 8D 6C 24 ?? 48 81 EC ?? ?? ?? ?? 49 8B D8");
static const auto pattern_UTIL_ClientPrintAll = THE_GAME_SIG(
    "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 81 EC 70 01 ?? ?? 8B "
    "E9");
static const auto pattern_FnClientPrint =
    THE_GAME_SIG("48 85 C9 0F 84 ?? ?? ?? ?? 48 8B C4 48 89 58 18");
static const auto pattern_CCSWeaponBase_Spawn = THE_GAME_SIG(
    "48 89 5C 24 08 48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 30 48 8B DA 48 "
    "8B E9 E8 ?? ?? ?? ??");
extern uint64_t GameResourceServicePtr;
extern uint64_t FireEventServerSidePtr;
extern uint64_t Module_tier0;
extern uint64_t Host_SayPtr;
extern uint64_t MaxPlayerNumsPtr;
extern uint64_t FnUTIL_SayText2FilterPtr;

extern HashFunction_t FnServerHashFunction;
extern StateChanged_t FnStateChanged;
extern NetworkStateChanged_t FnNetworkStateChanged;
extern RespawnPlayerInDeathMatch_t FnRespawnPlayerInDeathMatch;
extern GiveNamedItem_t FnGiveNamedItem;
extern EntityRemove_t FnEntityRemove;
extern UTIL_SayTextFilter_t FnUTIL_SayTextFilter;
extern UTIL_ClientPrintAll_t FnUTIL_ClientPrintAll;
extern ClientPrint_t FnClientPrint;
extern CCSWeaponBase_Spawn_t FnCCSWeaponBase_Spawn;
extern PlayerChangeName_t FnPlayerChangeName;
extern bool InitOffsetSuccess;
auto Init() -> bool;
};  // namespace Offset
