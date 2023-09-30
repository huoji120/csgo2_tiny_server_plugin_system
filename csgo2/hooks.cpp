#include "hooks.h"
#include "native_sdk/entity/cbaseentity.h"
#include "sdk/convar/convar.hpp"
FireEventServerSide_t original_FireEventServerSide = NULL;
OnClientConnect_t original_OnClientConnected = NULL;
Host_Say_t original_Host_Say = NULL;

namespace hooks {
    // "player_connect" 
    VMTHook* VMT_IServerGameClient;
    void __fastcall hook_OnClientConnected(void* rcx, CPlayerSlot slot, const char* pszName, uint64_t xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer)
    {
        LOG("OnClientConnected(%d, \"%s\", %d, \"%s\", \"%s\" \"%d\")\n", slot.Get(), pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
        return original_OnClientConnected(rcx, slot, pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
    }
    void __fastcall hook_Host_Say(void* pEntity, void* args, bool teamonly, int unk1, const char* unk2)
    {
        const auto theArgs = reinterpret_cast<CCommand*>(args);
        const auto theEntity = reinterpret_cast<CBaseEntity*>(pEntity);
        char* pos = nullptr;
        do
        {
            if (theArgs == nullptr || theEntity == nullptr) {
                break;
            }
            auto message = std::string(theArgs->GetCommandString());
            printf("messageNoZero: %s \n", message.c_str());
        } while (false);
        /*
        if (*pMessage == '!' || *pMessage == '/')
            ParseChatCommand(pMessage, pEntity);

        if (*pMessage == '/')
            return;
         */
        return original_Host_Say(pEntity, args, teamonly, unk1, unk2);
    }

    bool __fastcall hook_FireEventServerSide(CGameEventManager* rcx, IGameEvent* event,
        bool serverSide) {
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
            static constexpr auto player_chat =
                hash_32_fnv1a_const("player_chat");
            switch (hash_32_fnv1a_const(eventName))
            {
            case player_death:
                events::OnPlayerDeathEvent(event);
                break;
                //V社bug,这不会有用
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
            if (MH_CreateHook((LPVOID)Offset::Host_SayPtr,
                &hook_Host_Say,
                reinterpret_cast<LPVOID*>(
                    &original_Host_Say)) != MH_OK) {
                LOG("MH_CreateHook original_Host_Say\n");
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
        VMT_IServerGameClient = new VMTHook(Memory::read<void*>(reinterpret_cast<uint64_t>(Offset::InterFaces::IServerGameClient)));
        original_OnClientConnected = reinterpret_cast<OnClientConnect_t>(VMT_IServerGameClient->Hook(11, hook_OnClientConnected));

        LOG("%s original_OnClientConnected: %p \n", __FUNCTION__, original_OnClientConnected);
        return original_OnClientConnected != nullptr;
    }
	auto init() -> bool {
        bool isSuccess = initMinHook() && initVmtHook();
        //bool isSuccess = initVmtHook();
        return isSuccess;
	}
    auto unload() -> void
    {
        VMT_IServerGameClient->ClearHooks();

        delete VMT_IServerGameClient;

        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }
}