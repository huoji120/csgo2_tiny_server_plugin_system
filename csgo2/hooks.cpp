#include "hooks.h"
extern auto GetGameGlobals() -> CGlobalVars*;

namespace hooks {
// "player_connect"
VMTHook* VMT_IServerGameClient;
VMTHook* VMT_INetworkServerServiceInteFace;
VMTHook* VMT_ISource2ServerInterFace;
VMTHook* VMT_GameEventSystem;

FireEventServerSide_t original_FireEventServerSide = NULL;
OnClientConnect_t original_OnClientConnected = NULL;
OnClientDisconnect_t original_OnClientDisconnect = NULL;
Host_Say_t original_Host_Say = NULL;
StartupServer_t origin_StartServer = NULL;
GameFrame_t origin_GameFrame = NULL;
CCSWeaponBase_Spawn_t origin_CCSWeaponBase_Spawn = NULL;
UTIL_SayText2Filter_t origin_UTIL_SayText2Filter = NULL;
PostEventAbstract_t origin_PostEventAbstract = NULL;

void __fastcall hook_PostEventAbstract(
    void* rcx, 
    CSplitScreenSlot nSlot, 
    bool bLocalOnly, 
    int nClientCount, 
    const uint64* clients, 
    INetworkSerializable* pEvent, 
    const void* pData, 
    unsigned long nSize, 
    NetChannelBufType_t bufType)
{
    do
    {
        if (global::EntitySystem == nullptr) {
            break;
        }
        if (pEvent == nullptr) {
            break;
        }
        NetMessageInfo_t* info = pEvent->GetNetMessageInfo();
        if (info == nullptr) {
            break;
        }
        const auto isBloodAboutMessage = (info->m_MessageId == TE_WorldDecalId || info->m_MessageId == TE_EffectDispatchId);
        if (isBloodAboutMessage == false/* && isWeaponAboutMessage == false */) {
            break;
        }
        // lazy fix me
        for (uint64_t i = 0; i < global::MaxPlayers; i++)
        {
            if (!(*(uint64_t*)clients & ((uint64_t)1 << i))) {
                continue;
            }

            const auto pEntity = global::EntitySystem->GetBaseEntity(PlayerSlot_to_EntityIndex(i));
            if (pEntity == nullptr) {
                continue;
            }

            if (pEntity->IsBasePlayerController() == false) {
                continue;
            }
            const auto player = reinterpret_cast<CCSPlayerController*>(pEntity);

            const auto [isSuccess, playerSetting] = ExtendPlayerManager::GetPlayerSettingBySteamId(player->m_steamID());
            if (isSuccess == false) {
                continue;
            }
            bool skipTheEvent = false;
            if (isBloodAboutMessage) {
                skipTheEvent = (playerSetting.bloodSetting == _ExtendPlayerSetting_Blood::kDisableBloodEffectDispatch && info->m_MessageId == TE_EffectDispatchId) ||
                    (playerSetting.bloodSetting == _ExtendPlayerSetting_Blood::kDisableBloodWorldDecal && info->m_MessageId == TE_WorldDecalId) || 
                    (playerSetting.bloodSetting == _ExtendPlayerSetting_Blood::kDisableBloodWorldDecalAndEffectDispatch);
            }
            /*
            else if (isWeaponAboutMessage)
            {
                skipTheEvent = (playerSetting.weaponSetting == _ExtendPlayerSetting_Weapon::kDisablebulletHole);
            }
            */
            if (skipTheEvent) {
                *(uint64*)clients &= ~((uint64)1 << i);
                nClientCount--;
            }
        }
    } while (false);
    origin_PostEventAbstract(rcx, nSlot, bLocalOnly, nClientCount, clients, pEvent, pData, nSize, bufType);
}
void __fastcall hook_UTIL_SayText2Filter(
    IRecipientFilter& filter, CCSPlayerController* pEntity,
    uint64_t eMessageType, const char* messeageName, const char* param1,
    const char* param2, const char* param3, const char* param4) {
    const auto entIndex =
        PlayerSlot_to_EntityIndex(filter.GetRecipientIndex(0).Get());
    const auto isHandle = ScriptCallBacks::luCall_onSayText2Filter(
        entIndex, eMessageType, messeageName, param1, param2, param3, param4);
    if (isHandle == false) {
        origin_UTIL_SayText2Filter(filter, pEntity, eMessageType, messeageName,
                                   param1, param2, param3, param4);
    }
}
// https://github.com/Source2ZE/CS2Fixes/blob/main/src/commands.cpp#L494
void __fastcall hook_CCSWeaponBase_Spawn(CBaseEntity* pThis, void* a2) {
    const char* pszClassName = pThis->m_pEntity()->m_designerName;

    origin_CCSWeaponBase_Spawn(pThis, a2);
    if (pszClassName == nullptr) {
        return;
    }
    LOG("Weapon spawn: %s\n", pszClassName);

    do {
        auto pWeapon = reinterpret_cast<CCSWeaponBase*>(pThis);
        // Weapon could be already initialized with the correct data from
        // GiveNamedItem, in that case we don't need to do anything
        if (!pWeapon ||
            pWeapon->m_AttributeManager()->m_Item()->m_bInitialized()) {
            break;
        }
        const auto weaponName = Tools::toLower(std::string(pszClassName));
        auto lookupWeaponSimpleName = std::string();
        for (const auto& weapon : GameWeapons::WeaponMap) {
            const auto& key = weapon.first;
            const auto& [fullWeaponName, weaponItemDefIndex] = weapon.second;
            if (fullWeaponName.find(weaponName) == std::string::npos) {
                continue;
            }
            lookupWeaponSimpleName = key;
            break;
        }
        if (lookupWeaponSimpleName.size() <= 1) {
            break;
        }
        // lazy , fix me
        const auto [fullWeaponName, weaponiItemDefIndex] =
            GameWeapons::WeaponMap.at(lookupWeaponSimpleName);

        LOG("Fixing a %s with index = %d and initialized = %d\n",
            fullWeaponName.c_str(),
            pWeapon->m_AttributeManager()->m_Item()->m_iItemDefinitionIndex(),
            pWeapon->m_AttributeManager()->m_Item()->m_bInitialized());

        pWeapon->m_AttributeManager()->m_Item()->m_bInitialized(true);
        pWeapon->m_AttributeManager()->m_Item()->m_iItemDefinitionIndex(
            weaponiItemDefIndex);
    } while (false);
}

void __fastcall hook_GameFrame(void* rcx, bool simulating, bool bFirstTick,
                               bool bLastTick) {
    /**
     * simulating:
     * ***********
     * true  | game is ticking
     * false | game is not ticking
     */
    if (global::GlobalVars != nullptr) {
        if (simulating && global::HasTicked) {
            global::m_flUniversalTime +=
                global::GlobalVars->curtime - global::m_flLastTickedTime;
        } else {
            global::m_flUniversalTime += global::GlobalVars->interval_per_tick;
        }

        global::m_flLastTickedTime = global::GlobalVars->curtime;
        global::HasTicked = true;

        GameTimer::ExcuteTimers();
        GameTickRunTime::ExcuteTickFunctions();
    }
    if (global::EntitySystem == nullptr) {
        global::EntitySystem = CGameEntitySystem::GetInstance();
    }
    if (global::GlobalVars == nullptr) {
        global::GlobalVars = GetGameGlobals();
    }
    return origin_GameFrame(rcx, simulating, bFirstTick, bLastTick);
}
void __fastcall hook_StartServer(void* rcx,
                                 const GameSessionConfiguration_t& config,
                                 ISource2WorldSession* pWorldSession,
                                 const char* a1) {
    if (global::HasTicked) {
        GameTimer::CleanUpTimers();
    }

    global::HasTicked = false;
    global::GlobalVars = GetGameGlobals();
    global::EntitySystem =
        Offset::InterFaces::GameResourceServiceServer->GetGameEntitySystem();

    return origin_StartServer(rcx, config, pWorldSession, a1);
}
void __fastcall hook_ClientDisconnect(void* rcx, CPlayerSlot slot, int reason,
                                      const char* pszName, uint64_t xuid,
                                      const char* pszNetworkID) {
    bool isFakePlayer = true;
    if (pszNetworkID != NULL && *pszNetworkID == '[') {
        ExtendPlayerManager::RemovePlayerSlotBySteamId(
            ExtendPlayerManager::SteamIDStringToUInt64(pszNetworkID));
        isFakePlayer = false;
    }
    events::OnPlayerDisconnect(slot.Get(), pszName, xuid, pszNetworkID,
                               pszNetworkID, isFakePlayer);
    return original_OnClientDisconnect(rcx, slot, reason, pszName, xuid,
                                       pszNetworkID);
}
void __fastcall hook_OnClientConnected(void* rcx, CPlayerSlot slot,
                                       const char* pszName, uint64_t xuid,
                                       const char* pszNetworkID,
                                       const char* pszAddress,
                                       bool bFakePlayer) {
    if (bFakePlayer == false) {
        ExtendPlayerManager::AddSteamIdToPlayerSteamIdWithNameTable(
            ExtendPlayerManager::SteamIDStringToUInt64(pszNetworkID),
            slot.Get());
    }
    events::OnPlayerConnect(slot.Get(), pszName, xuid, pszNetworkID, pszAddress,
                            bFakePlayer);
    return original_OnClientConnected(rcx, slot, pszName, xuid, pszNetworkID,
                                      pszAddress, bFakePlayer);
}
void __fastcall hook_Host_Say(void* pEntity, void* args, bool teamonly,
                              int unk1, const char* unk2) {
    const auto theArgs = reinterpret_cast<CCommand*>(args);
    const auto theEntity = reinterpret_cast<CCSPlayerController*>(pEntity);

    char* pos = nullptr;
    bool blockMsg = false;
    do {
        if (theArgs == nullptr) {
            break;
        }
        const auto message = std::string(theArgs->GetCommandString());
        if (theEntity == nullptr) {
            if (events::OnConsoleChat(message) == true) {
                blockMsg = true;
                break;
            }
            break;
        }
        if (events::OnPlayerChat(theEntity, message) == true) {
            blockMsg = true;
            break;
        }
    } while (false);
    /*
    if (*pMessage == '!' || *pMessage == '/')
        ParseChatCommand(pMessage, pEntity);

    if (*pMessage == '/')
        return;
     */
    if (blockMsg) {
        return;
    } else {
        return original_Host_Say(pEntity, args, teamonly, unk1, unk2);
    }
}

bool __fastcall hook_FireEventServerSide(CGameEventManager* rcx,
                                         IGameEvent* event, bool serverSide) {
    do {
        if (!event) {
            break;
        }

        const char* eventName = event->GetName();
        if (!eventName) {
            break;
        }
        static constexpr auto player_death =
            hash_32_fnv1a_const("player_death");
        static constexpr auto player_chat = hash_32_fnv1a_const("player_chat");
        static constexpr auto player_spawn =
            hash_32_fnv1a_const("player_spawn");
        static constexpr auto round_start = hash_32_fnv1a_const("round_start");
        static constexpr auto round_end = hash_32_fnv1a_const("round_end");
        static constexpr auto player_hurt = hash_32_fnv1a_const("player_hurt");
        static constexpr auto player_team = hash_32_fnv1a_const("player_team");

        switch (hash_32_fnv1a_const(eventName)) {
            case player_death:
                events::OnPlayerDeathEvent(event);
                break;
            case player_spawn:
                events::OnPlayerSpawnEvent(event);
                break;
            case round_start:
                events::OnRoundStartEvent(event);
                break;
            case round_end:
                events::OnRoundEndEvent(event);
                break;
            case player_hurt:
                events::OnPlayerHurtEvent(event);
                break;
            case player_team:
                events::OnPlayerTeamChangeEevent(event);
                break;
                // V社bug,这不会有用
                /*
                case player_chat:
                    events::OnPlayerChat(event);
                    break;
                }
                */
        }

    } while (false);
    return original_FireEventServerSide(rcx, event, serverSide);
}
auto initMinHook() -> bool {
    bool isSuccess = false;
    // 初始化MiniHook
    do {
        if (MH_Initialize() != MH_OK) {
            LOG("MH_Initialize() != MH_OK\n");
            break;
        }
        // 创建钩子
        if (MH_CreateHook((LPVOID)Offset::FireEventServerSidePtr,
                          &hook_FireEventServerSide,
                          reinterpret_cast<LPVOID*>(
                              &original_FireEventServerSide)) != MH_OK) {
            LOG("MH_CreateHook original_FireEventServerSide\n");
            break;
        }
        if (MH_CreateHook((LPVOID)Offset::Host_SayPtr, &hook_Host_Say,
                          reinterpret_cast<LPVOID*>(&original_Host_Say)) !=
            MH_OK) {
            LOG("MH_CreateHook original_Host_Say\n");
            break;
        }
        if (MH_CreateHook((LPVOID)Offset::FnCCSWeaponBase_Spawn,
                          &hook_CCSWeaponBase_Spawn,
                          reinterpret_cast<LPVOID*>(
                              &origin_CCSWeaponBase_Spawn)) != MH_OK) {
            LOG("MH_CreateHook origin_CCSWeaponBase_Spawn\n");
            break;
        }
        if (MH_CreateHook((LPVOID)Offset::FnUTIL_SayText2FilterPtr,
                          &hook_UTIL_SayText2Filter,
                          reinterpret_cast<LPVOID*>(
                              &origin_UTIL_SayText2Filter)) != MH_OK) {
            LOG("MH_CreateHook origin_UTIL_SayText2Filter\n");
            break;
        }
        // 启用钩子
        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
            LOG("MH_EnableHook \n");
            break;
        }
        isSuccess = true;
    } while (false);
    return isSuccess;
}
auto initVmtHook() -> bool {
    VMT_IServerGameClient = new VMTHook(Memory::read<void*>(
        reinterpret_cast<uint64_t>(Offset::InterFaces::IServerGameClient)));
    VMT_INetworkServerServiceInteFace =
        new VMTHook(Memory::read<void*>(reinterpret_cast<uint64_t>(
            Offset::InterFaces::INetworkServerServiceInteFace)));
    VMT_ISource2ServerInterFace =
        new VMTHook(Memory::read<void*>(reinterpret_cast<uint64_t>(
            Offset::InterFaces::ISource2ServerInterFace)));
    VMT_GameEventSystem =
        new VMTHook(Memory::read<void*>(reinterpret_cast<uint64_t>(
            Offset::InterFaces::GameEventSystem)));

    origin_PostEventAbstract = reinterpret_cast<PostEventAbstract_t>(
        VMT_GameEventSystem->Hook(16, hook_PostEventAbstract));
    original_OnClientConnected = reinterpret_cast<OnClientConnect_t>(
        VMT_IServerGameClient->Hook(11, hook_OnClientConnected));
    original_OnClientDisconnect = reinterpret_cast<OnClientDisconnect_t>(
        VMT_IServerGameClient->Hook(16, hook_ClientDisconnect));

    origin_StartServer = reinterpret_cast<StartupServer_t>(
        VMT_INetworkServerServiceInteFace->Hook(27, hook_StartServer));
    origin_GameFrame = reinterpret_cast<GameFrame_t>(
        VMT_ISource2ServerInterFace->Hook(19, hook_GameFrame));
    return original_OnClientConnected && original_OnClientDisconnect &&
           origin_StartServer && origin_GameFrame;
}
auto initConVarHooks() -> void {
    // Offset::InterFaces::IVEngineCvar->RegisterConVar
}
auto init() -> bool {
    bool isSuccess = initMinHook() && initVmtHook();
    // bool isSuccess = initVmtHook();
    return isSuccess;
}
auto unload() -> void {
    VMT_IServerGameClient->ClearHooks();
    VMT_INetworkServerServiceInteFace->ClearHooks();
    VMT_ISource2ServerInterFace->ClearHooks();
    VMT_GameEventSystem->ClearHooks();

    delete VMT_IServerGameClient;
    delete VMT_INetworkServerServiceInteFace;
    delete VMT_ISource2ServerInterFace;
    delete VMT_GameEventSystem;

    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
}  // namespace hooks
