#include "offset.h"

namespace Offset {
uint64_t GameResourceServicePtr;
uint64_t FireEventServerSidePtr;
uint64_t CGameEventManagerPtr;
uint64_t CCSGameRulesInterFacePtr;
uint64_t Host_SayPtr;
uint64_t Module_tier0;
uint64_t MaxPlayerNumsPtr;
uint64_t FnUTIL_SayText2FilterPtr;
PlayerChangeName_t FnPlayerChangeName;
HashFunction_t FnServerHashFunction;
StateChanged_t FnStateChanged;
NetworkStateChanged_t FnNetworkStateChanged;
RespawnPlayerInDeathMatch_t FnRespawnPlayerInDeathMatch;
GiveNamedItem_t FnGiveNamedItem;
EntityRemove_t FnEntityRemove;
UTIL_SayTextFilter_t FnUTIL_SayTextFilter;
UTIL_ClientPrintAll_t FnUTIL_ClientPrintAll;
ClientPrint_t FnClientPrint;
CCSWeaponBase_Spawn_t FnCCSWeaponBase_Spawn;
// CreateGameRuleInterFace_t FnCreateCCSGameRulesInterFace;
bool InitOffsetSuccess = false;
namespace InterFaces {
CSchemaSystem* SchemaSystem;
IGameEventManager2* GameEventManager;
CGameEventManager* CGameEventManger;
CGameResourceService* GameResourceServiceServer;
IServerGameClients* IServerGameClient;
IVEngineServer2* IVEngineServer;
ISource2Server* ISource2ServerInterFace;
CLocalize* ILocalize;
INetworkServerService* INetworkServerServiceInteFace;
CCSGameRules* CCSGameRulesInterFace;
ICvar* IVEngineCvar;
IGameEventSystem* GameEventSystem;
};  // namespace InterFaces
auto SafeDelayInit(void* ctx) -> void {
    // 需要游戏调用函数初始化
    InterFaces::CCSGameRulesInterFace = reinterpret_cast<CCSGameRules*>(
        Memory::read<CCSGameRules*>(CCSGameRulesInterFacePtr));

    while (InterFaces::CCSGameRulesInterFace == 0) {
        InterFaces::CCSGameRulesInterFace = reinterpret_cast<CCSGameRules*>(
            Memory::read<CCSGameRules*>(CCSGameRulesInterFacePtr));
        Sleep(100);
    }
    InitOffsetSuccess = true;
    LOG("[huoji]InterFaces::CCSGameRulesInterFace : %llx \n",
        InterFaces::CCSGameRulesInterFace);
    LOG("m_bForceTeamChangeSilent: %d \n",
        InterFaces::CCSGameRulesInterFace->m_bForceTeamChangeSilent());
}

auto Init() -> bool {
    CModule server("server.dll");
    CModule schemasystem("schemasystem.dll");
    CModule engine("engine2.dll");
    CModule localize("localize.dll");
    CModule tier0("tier0.dll");
    Memory::PathVscript();

    // engine.dll
    engine.FindPattern(pattern_MaxPlayerNumsPtr)
        .ToAbsolute(3, 0)
        .Get(MaxPlayerNumsPtr);

    // server.dll
    server.FindPattern(pattern_FireEventServerSide).Get(FireEventServerSidePtr);
    server.FindPattern(pattern_NetworkStateChanged).Get(FnNetworkStateChanged);
    server.FindPattern(pattern_FnStateChangedPtr).Get(FnStateChanged);
    server.FindPattern(pattern_PlayerChangeName).Get(FnPlayerChangeName);

    server.FindPattern(pattern_CGameEventManager)
        .ToAbsolute(3, 0)
        .Get(CGameEventManagerPtr);
    server.FindPattern(pattern_CreateCCSGameRulesInterFacePtr)
        .ToAbsolute(3, 0)
        .Get(CCSGameRulesInterFacePtr);
    server.FindPattern(pattern_FnRespawnPlayerInDeathMatch)
        .Get(FnRespawnPlayerInDeathMatch);
    server.FindPattern(pattern_FnEntityRemove).Get(FnEntityRemove);
    server.FindPattern(pattern_FnGiveNamedItemPtr).Get(FnGiveNamedItem);
    server.FindPattern(pattern_fnHost_SayPtr).Get(Host_SayPtr);
    // server.FindPattern(pattern_ServerHashFunctionPtr).Get(FnServerHashFunction);
    server.FindPattern(pattern_UTIL_ClientPrintAll).Get(FnUTIL_ClientPrintAll);
    server.FindPattern(pattern_FnClientPrint).Get(FnClientPrint);

    server.FindPattern(pattern_FnUTIL_SayTextFilter).Get(FnUTIL_SayTextFilter);
    server.FindPattern(pattern_CCSWeaponBase_Spawn).Get(FnCCSWeaponBase_Spawn);
    server.FindPattern(pattern_FnUTIL_SayText2FilterPtr)
        .Get(FnUTIL_SayText2FilterPtr);

    InterFaces::SchemaSystem = reinterpret_cast<CSchemaSystem*>(
        schemasystem.FindInterface("SchemaSystem_001").Get());
    // InterFaces::GameEventManager = reinterpret_cast<IGameEventManager2*>(
    //     engine.FindInterface("GameEventSystemServerV001").Get());
    InterFaces::ILocalize = reinterpret_cast<CLocalize*>(
        localize.FindInterface("Localize_001").Get());
    InterFaces::IVEngineCvar =
        reinterpret_cast<ICvar*>(tier0.FindInterface("VEngineCvar007").Get());

    InterFaces::GameResourceServiceServer =
        reinterpret_cast<CGameResourceService*>(
            engine.FindInterface("GameResourceServiceServerV001").Get());
    InterFaces::IVEngineServer = reinterpret_cast<IVEngineServer2*>(
        engine.FindInterface("Source2EngineToServer001").Get());
    InterFaces::INetworkServerServiceInteFace =
        reinterpret_cast<INetworkServerService*>(
            engine.FindInterface("NetworkServerService_001").Get());
    InterFaces::GameEventSystem = reinterpret_cast<IGameEventSystem*>(
        engine.FindInterface("GameEventSystemServerV001").Get());
    InterFaces::IServerGameClient = reinterpret_cast<IServerGameClients*>(
        server.FindInterface("Source2GameClients001").Get());
    InterFaces::ISource2ServerInterFace = reinterpret_cast<ISource2Server*>(
        server.FindInterface("Source2Server001").Get());

    if (InterFaces::ISource2ServerInterFace) {
        InterFaces::GameEventManager =
            (IGameEventManager2*)(CALL_VIRTUAL(
                                      uintptr_t, 91,
                                      InterFaces::ISource2ServerInterFace) -
                                  8);
    }
    InterFaces::CGameEventManger =
        reinterpret_cast<CGameEventManager*>(CGameEventManagerPtr);

    // global::MaxPlayers = *(int*)((char*)MaxPlayerNumsPtr + 2);
    //  client.FindPattern(pattern_FireEventServerSide).Get(FireEventServerSidePtr);
    global::MaxPlayers = 64;

    LOG("[huoji]FireEventServerSidePtr : %llx \n", FireEventServerSidePtr);
    LOG("[huoji]Host_SayPtr : %llx \n", Host_SayPtr);
    LOG("[huoji]FnNetworkStateChanged : %llx \n", FnNetworkStateChanged);
    // LOG("[huoji]FnServerHashFunction : %llx \n", FnServerHashFunction);
    LOG("[huoji]FnStateChanged : %llx \n", FnStateChanged);
    LOG("[huoji]FnRespawnPlayerInDeathMatch : %llx \n",
        FnRespawnPlayerInDeathMatch);
    LOG("[huoji]FnGiveNamedItem : %llx \n", FnGiveNamedItem);
    LOG("[huoji]FnClientPrint : %llx \n", FnClientPrint);
    LOG("[huoji]FnUTIL_ClientPrintAll : %llx \n", FnUTIL_ClientPrintAll);
    LOG("[huoji]FnCCSWeaponBase_Spawn : %llx \n", FnCCSWeaponBase_Spawn);
    LOG("[huoji]FnPlayerChangeName : %llx \n", FnPlayerChangeName);
    LOG("[huoji]FnUTIL_SayText2FilterPtr : %llx \n", FnUTIL_SayText2FilterPtr);

    LOG("[huoji]MaxGlobals : %d \n", global::MaxPlayers);

    LOG("[huoji]InterFaces::SchemaSystem : %llx \n", InterFaces::SchemaSystem);
    LOG("[huoji]InterFaces::GameEventManager : %llx \n",
        InterFaces::GameEventManager);
    LOG("[huoji]InterFaces::CGameEventManger : %llx \n",
        InterFaces::CGameEventManger);
    LOG("[huoji]InterFaces::GameResourceServiceServer : %llx \n",
        InterFaces::GameResourceServiceServer);
    LOG("[huoji]InterFaces::IServerGameClient : %llx \n",
        InterFaces::IServerGameClient);
    LOG("[huoji]InterFaces::IVEngineServer : %llx \n",
        InterFaces::IVEngineServer);
    LOG("[huoji]InterFaces::ISource2ServerInterFace : %llx \n",
        InterFaces::ISource2ServerInterFace);
    LOG("[huoji]InterFaces::IVEngineCvar : %llx \n", InterFaces::IVEngineCvar);
    LOG("[huoji]InterFaces::GameEventSystem : %llx \n", InterFaces::GameEventSystem);

    LOG("[huoji] CGameEntitySystem::GetInstance : %llx \n",
        CGameEntitySystem::GetInstance());
    LOG("init offset success !\n");
    CreateThread(NULL, 0,
                 reinterpret_cast<LPTHREAD_START_ROUTINE>(SafeDelayInit), NULL,
                 0, NULL);
    //  LOG("FnServerHashFunction: %llx \n", FnServerHashFunction("here",
    //  sizeof("here") - 1, 0x31415926));
    return FnPlayerChangeName && FnCCSWeaponBase_Spawn && FnEntityRemove &&
           FnRespawnPlayerInDeathMatch && FnGiveNamedItem && Host_SayPtr &&
           InterFaces::GameEventSystem &&
           InterFaces::IVEngineServer &&
           InterFaces::GameResourceServiceServer &&
           InterFaces::IServerGameClient && InterFaces::GameEventManager &&
           InterFaces::SchemaSystem && FireEventServerSidePtr &&
           FnNetworkStateChanged;
}
}  // namespace Offset
