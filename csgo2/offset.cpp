#include "offset.h"

namespace Offset {
uint64_t GameResourceServicePtr;
uint64_t FireEventServerSidePtr;
uint64_t NetworkStateChangedPtr;
uint64_t CGameEventManagerPtr;
uint64_t Host_SayPtr;
uint64_t Module_tier0;
namespace InterFaces {
CSchemaSystem* SchemaSystem;
IGameEventManager2* GameEventManager;
CGameEventManager* CGameEventManger;
CGameResourceService* GameResourceServiceServer;
IServerGameClients* IServerGameClient;
IVEngineServer2* IVEngineServer;
};  // namespace InterFaces
auto Init() -> bool {
    CModule server("server.dll");
    CModule schemasystem("schemasystem.dll");
    CModule engine("engine2.dll");

    // server.dll
    server.FindPattern(pattern_FireEventServerSide).Get(FireEventServerSidePtr);
    server.FindPattern(pattern_NetworkStateChanged).Get(NetworkStateChangedPtr);
    // 48 8D 05 4A 30 82 00    lea     rax, ??_7CGameEventManager@@6B@
    server.FindPattern(pattern_CGameEventManager)
        .ToAbsolute(3, 0)
        .Get(CGameEventManagerPtr);
    server.FindPattern(pattern_fnHost_SayPtr).Get(Host_SayPtr);

    // schemasystem
    InterFaces::SchemaSystem = reinterpret_cast<CSchemaSystem*>(
        schemasystem.FindInterface("SchemaSystem_001").Get());
    // engine.dll
    InterFaces::GameEventManager = reinterpret_cast<IGameEventManager2*>(
        engine.FindInterface("GameEventSystemServerV001").Get());
    InterFaces::GameResourceServiceServer =
        reinterpret_cast<CGameResourceService*>(
            engine.FindInterface("GameResourceServiceServerV001").Get());
    InterFaces::IVEngineServer = reinterpret_cast<IVEngineServer2*>(
        engine.FindInterface("Source2EngineToServer001").Get());

    // server.dll
    InterFaces::IServerGameClient = reinterpret_cast<IServerGameClients*>(
        server.FindInterface("Source2GameClients001").Get());

    // only init in console server
    InterFaces::CGameEventManger =
        reinterpret_cast<CGameEventManager*>(CGameEventManagerPtr);

    // client.FindPattern(pattern_FireEventServerSide).Get(FireEventServerSidePtr);
    LOG("[huoji]FireEventServerSidePtr : %llx \n", FireEventServerSidePtr);
    LOG("[huoji]NetworkStateChangedPtr : %llx \n", NetworkStateChangedPtr);
    LOG("[huoji]Host_SayPtr : %llx \n", Host_SayPtr);

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

    // GetOffsets();
    LOG("init offset success !\n");
    return Host_SayPtr && InterFaces::IVEngineServer &&
           InterFaces::GameResourceServiceServer &&
           InterFaces::IServerGameClient && InterFaces::GameEventManager &&
           InterFaces::SchemaSystem && FireEventServerSidePtr &&
           NetworkStateChangedPtr;
}
}  // namespace Offset
