#include "hooks.h"

FireEventServerSide_t original_FireEventServerSide = NULL;
OnClientConnect_t original_OnClientConnected = NULL;
OnClientDisconnect_t original_OnClientDisconnect = NULL;
Host_Say_t original_Host_Say = NULL;

namespace hooks {
// "player_connect"
VMTHook* VMT_IServerGameClient;
void __fastcall hook_ClientDisconnect(void* rcx, CPlayerSlot slot, int reason,
                                      const char* pszName, uint64_t xuid,
                                      const char* pszNetworkID) {
    if (pszNetworkID != NULL && *pszNetworkID == '[') {
        ExtendPlayerManager::RemovePlayerSlotBySteamId(
            ExtendPlayerManager::SteamIDStringToUInt64(pszNetworkID));
    }
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
        if (theArgs == nullptr || theEntity == nullptr) {
            break;
        }        
        const auto message = std::string(theArgs->GetCommandString());

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
    if (blockMsg) { return; }
    else {
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
        switch (hash_32_fnv1a_const(eventName)) {
            case player_death:
                events::OnPlayerDeathEvent(event);
                break;
                // V��bug,�ⲻ������
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
    // ��ʼ��MiniHook
    do {
        if (MH_Initialize() != MH_OK) {
            LOG("MH_Initialize() != MH_OK\n");
            break;
        }
        // ��������
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
        // ���ù���
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
    original_OnClientConnected = reinterpret_cast<OnClientConnect_t>(
        VMT_IServerGameClient->Hook(11, hook_OnClientConnected));
    original_OnClientDisconnect = reinterpret_cast<OnClientDisconnect_t>(
        VMT_IServerGameClient->Hook(16, hook_ClientDisconnect));
    return original_OnClientConnected && original_OnClientDisconnect;
}
auto init() -> bool {
    bool isSuccess = initMinHook() && initVmtHook();
    // bool isSuccess = initVmtHook();
    return isSuccess;
}
auto unload() -> void {
    VMT_IServerGameClient->ClearHooks();

    delete VMT_IServerGameClient;

    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
}  // namespace hooks