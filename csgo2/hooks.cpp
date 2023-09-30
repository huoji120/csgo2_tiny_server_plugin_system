#include "hooks.h"
#include "native_sdk/entity/cbaseentity.h"
#include "sdk/convar/convar.hpp"
FireEventServerSide_t original_FireEventServerSide = NULL;
OnClientConnect_t original_OnClientConnected = NULL;
Host_Say_t original_Host_Say = NULL;

namespace hooks {
    VTHook* HIServerGameClient;
    bool __fastcall hook_OnClientConnected(CPlayerSlot slot, const char* pszName, uint64_t xuid, const char* pszNetworkID, bool unk1, CBufferString* pRejectReason)
    {
        LOG("Hook_OnClientConnected(%d, \"%s\", %d, \"%s\")\n", slot, pszName, xuid, pszNetworkID);
        return original_OnClientConnected(slot, pszName, xuid, pszNetworkID, unk1, pRejectReason);
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

        HIServerGameClient = new VTHook((DWORD64**)Offset::InterFaces::IServerGameClient);
        original_OnClientConnected = (OnClientConnect_t)HIServerGameClient->HookFunction((DWORD64)hook_OnClientConnected, 1);
        return original_OnClientConnected != nullptr;
    }
	auto init() -> bool {
        bool isSuccess = initMinHook() && initVmtHook();
        //bool isSuccess = initVmtHook();
        return isSuccess;
	}
    auto unload() -> void
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }
}