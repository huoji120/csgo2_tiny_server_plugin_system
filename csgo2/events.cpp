#include "events.h"
#include "native_sdk/entity/cbaseplayercontroller.h"
#include "player.h"
#include "player_manager.h"

namespace events {
auto OnPlayerDeathEvent(IGameEvent* event) -> void {
    UnkGameEventStruct_t userIdNameParams{"userid"};
    UnkGameEventStruct_t attackerNameParams{"attacker"};

    const auto victim = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(&userIdNameParams));
    const auto attacker = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(&attackerNameParams));

    CGameEntitySystem* pEntitySystem = CGameEntitySystem::GetInstance();
    if (!pEntitySystem) return;

    for (int i = 1; i <= 64; ++i) {
        CBaseEntity* pEntity = pEntitySystem->GetBaseEntity(i);
        if (!pEntity) continue;
        if (pEntity->IsBasePlayerController()) {
            const auto player = reinterpret_cast<CCSPlayerController*>(pEntity);
            if (player->m_hPawn().Get() == victim) {
                printf("Found Pawn Player: %d %s \n", player->GetRefEHandle().GetEntryIndex(),&player->m_iszPlayerName());
                break;
            }
        }
    }
    //printf("player[%p] %s kill[%p] %llu\n", attacker, &attacker->m_iszPlayerName(), victim,  &victim->m_steamID());
}
auto OnPlayerChat(IGameEvent* event) -> void {}
}  // namespace events
