#include "events.h"
#include "native_sdk/entity/cbaseplayercontroller.h"
#include "player.h"
namespace events {
    auto OnPlayerDeathEvent(IGameEvent* event) -> void {
        UnkGameEventStruct_t userIdNameParams{ "userid" };
        UnkGameEventStruct_t attackerNameParams{ "attacker" };

        const auto victim = reinterpret_cast<CCSPlayerController*>(event->GetPlayerPawn(&userIdNameParams));
        const auto attacker = reinterpret_cast<CCSPlayerController*>(event->GetPlayerPawn(&attackerNameParams));
        auto victimName = &victim->m_iszPlayerName();
        auto attackerName = &attacker->m_iszPlayerName();

        //victimBasePlayer->ForceRespawn();
        printf("victim %s\n", victimName);
        printf("attacker %s\n", attackerName);

    }
    auto OnPlayerChat(IGameEvent* event) -> void
    {
        UnkGameEventStruct_t userIdNameParams{ "userid" };
        const auto chater = reinterpret_cast<CCSPlayerController*>(event->GetPlayer(&userIdNameParams));
        const auto text = event->GetString("text");
        const auto chaterName = chater->m_iszPlayerName();

        LOG("player: %s say: %s \n", chaterName, text);


    }
}