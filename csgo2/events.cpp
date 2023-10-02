#include "events.h"

namespace events {
auto OnPlayerDeathEvent(IGameEvent* event) -> void {
    UnkGameEventStruct_t userIdNameParams{"userid"};
    UnkGameEventStruct_t attackerNameParams{"attacker"};

    const auto victim = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(&userIdNameParams));
    const auto attacker = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(&attackerNameParams));

    //printf("player[%p] %s kill[%p] %llu\n", attacker, &attacker->m_iszPlayerName(), victim,  &victim->m_steamID());
}
auto OnPlayerChat(CCSPlayerController* player, std::string message) -> bool {
    auto [procesChatSuccess, chatType, chatCtx] = SdkTools::ProcessChatString(message);
    if (procesChatSuccess == false) {
        return false;
    }

    LOG("player %s say[%d]: %s steamid: %llu\n", &player->m_iszPlayerName(), chatType ,chatCtx.c_str(), player->m_steamID());
    if (chatCtx.at(0) == '/' || chatCtx.at(0) == '!') {
        return true;
    }
    return false;
}
}  // namespace events
