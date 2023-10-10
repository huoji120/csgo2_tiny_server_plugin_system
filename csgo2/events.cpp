#include "events.h"

namespace events {
auto OnPlayerTeamChangeEevent(IGameEvent* event) -> void {
    GameEventKeySymbol_t userIdNameParams{ "userid" };
    GameEventKeySymbol_t teamNameParams{ "team" };
    GameEventKeySymbol_t oldteamNameParams{ "oldteam" };
    GameEventKeySymbol_t disconnectNameParams{ "disconnect"};
    GameEventKeySymbol_t silentNameParams{ "silent" };
    GameEventKeySymbol_t isbotParams{ "isbot"};

    const auto PlayerPawn = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(userIdNameParams));
    if (PlayerPawn == nullptr) {
        return;
    }
    if (PlayerPawn->IsBasePlayerController() == false) {
        return;
    }
    const auto Player = PlayerPawn->GetPlayerController();
    if (Player == nullptr) {
        return;
    }
    const auto playerIndex = Player->GetRefEHandle().GetEntryIndex();
    auto team = event->GetInt(teamNameParams);
    auto oldTeam = event->GetInt(oldteamNameParams);
    auto disconnect = event->GetBool(disconnectNameParams);
    auto slient = event->GetBool(silentNameParams);
    auto isBot = event->GetBool(isbotParams);
    if (ScriptCallBacks::luaCall_onPlayerTeamChange(playerIndex, team, oldTeam, disconnect, slient, isBot) == true) {
        event->SetBool(silentNameParams, true);
    }
}
auto OnPlayerHurtEvent(IGameEvent* event) -> void {
    /*
    auto luaCall_onPlayerHurt(int userid, int attacker, int health, int armor,
                          const char* weapon, int dmg_health, int dmg_armor,
                          int hitgroup) -> void
    */
    GameEventKeySymbol_t userIdNameParams{"userid"};
    GameEventKeySymbol_t attackerNameParams{"attacker"};
    GameEventKeySymbol_t healthNameParams{"health"};
    GameEventKeySymbol_t armorNameParams{"armor"};
    GameEventKeySymbol_t weaponNameParams{"weapon"};
    GameEventKeySymbol_t dmg_healthNameParams{"dmg_health"};
    GameEventKeySymbol_t dmg_armorNameParams{"dmg_armor"};
    GameEventKeySymbol_t hitgroupNameParams{"hitgroup"};

    const auto victimPawn = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(userIdNameParams));
    const auto attackerPawn = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(attackerNameParams));
    if (victimPawn == nullptr || attackerPawn == nullptr) {
        return;
    }
    if (victimPawn->IsBasePlayerController() == false ||
        attackerPawn->IsBasePlayerController() == false) {
        return;
    }
    const auto victim = victimPawn->GetPlayerController();
    const auto attacker = attackerPawn->GetPlayerController();
    if (victim == nullptr || attacker == nullptr) {
        return;
    }
    const auto victimIndex = victim->GetRefEHandle().GetEntryIndex();
    const auto attackerIndex = attacker->GetRefEHandle().GetEntryIndex();

    auto health = event->GetInt(healthNameParams);
    auto armor = event->GetInt(armorNameParams);
    auto weapon = event->GetString(weaponNameParams);
    auto dmg_health = event->GetInt(dmg_healthNameParams);
    auto dmg_armor = event->GetInt(dmg_armorNameParams);
    auto hitgroup = event->GetInt(hitgroupNameParams);
    ScriptCallBacks::luaCall_onPlayerHurt(victimIndex, attackerIndex, health,
                                          armor, weapon, dmg_health, dmg_armor,
                                          hitgroup);
}
auto OnRoundEndEvent(IGameEvent* event) -> void {
    /*
        "winner"	"byte"		// winner team/user i
        "reason"	"byte"		// reson why team won
        "message"	"string"	// end round message
    */

    GameEventKeySymbol_t winnerNameParams{"winner"};
    GameEventKeySymbol_t reasonNameParams{"reason"};
    GameEventKeySymbol_t messageNameParams{"message"};

    const auto message = event->GetString(messageNameParams);
    const auto winner = event->GetInt(winnerNameParams);
    const auto reason = event->GetInt(reasonNameParams);

    ScriptCallBacks::luaCall_onRoundEnd(winner, reason, message);
}
auto OnRoundStartEvent(IGameEvent* event) -> void {
    GameEventKeySymbol_t timelimitNameParams{"timelimit"};
    const auto timelimit = event->GetInt(timelimitNameParams);
    ScriptCallBacks::luaCall_onRoundStart(timelimit);
}
auto OnPlayerSpawnEvent(IGameEvent* event) -> void {
    GameEventKeySymbol_t userIdNameParams{"userid"};
    const auto playerPawn = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(userIdNameParams));
    if (playerPawn == nullptr) {
        return;
    }
    const auto player = playerPawn->GetPlayerController();
    if (player == nullptr) {
        return;
    }
    const auto playerIndex = player->GetRefEHandle().GetEntryIndex();
    ScriptCallBacks::luaCall_onPlayerSpawn(playerIndex);
}
auto OnPlayerDeathEvent(IGameEvent* event) -> void {
    GameEventKeySymbol_t userIdNameParams{"userid"};
    GameEventKeySymbol_t attackerNameParams{"attacker"};
    GameEventKeySymbol_t headshotNameParams{"headshot"};
    
    const auto victimPawn = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(userIdNameParams));
    const auto attackerPawn = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(attackerNameParams));
    const auto isHeadShot = event->GetBool(headshotNameParams);
    if (victimPawn == nullptr || attackerPawn == nullptr) {
        return;
    }
    if (victimPawn->IsBasePlayerController() == false ||
        attackerPawn->IsBasePlayerController() == false) {
        return;
    }
    const auto victim = victimPawn->GetPlayerController();
    const auto attacker = attackerPawn->GetPlayerController();
    if (victim == nullptr || attacker == nullptr) {
        return;
    }
    const auto victimIndex = victim->GetRefEHandle().GetEntryIndex();
    const auto attackerIndex = attacker->GetRefEHandle().GetEntryIndex();
    ScriptCallBacks::luaCall_onPlayerDeath(victimIndex, attackerIndex,
                                           isHeadShot);
    //printf("player[%p] %s kill[%p] %llu\n", attacker, &attacker->m_iszPlayerName(), victim,  &victim->m_steamID());
}
auto OnPlayerChat(CCSPlayerController* player, std::string message) -> bool {
    auto [procesChatSuccess, chatType, chatCtx] =
        SdkTools::ProcessChatString(message);
    if (procesChatSuccess == false) {
        return false;
    }
    return ScriptCallBacks::luaCall_onPlayerSpeak(
        player->GetRefEHandle().GetEntryIndex(), chatType, chatCtx);
}
auto OnPlayerConnect(int slot, const char* pszName, uint64_t xuid,
                     const char* pszNetworkID, const char* pszAddress,
                     bool bFakePlayer) -> void {
    const auto PlayerIndex = PlayerSlot_to_EntityIndex(slot);
    ScriptCallBacks::luaCall_onPlayerConnect(PlayerIndex, slot, pszName, xuid,
                                             pszNetworkID, pszAddress,
                                             bFakePlayer);
}
auto OnPlayerDisconnect(int slot, const char* pszName, uint64_t xuid,
                        const char* pszNetworkID, const char* pszAddress,
                        bool bFakePlayer) -> void {
    const auto PlayerIndex = PlayerSlot_to_EntityIndex(slot);
    ScriptCallBacks::luaCall_onPlayerDisconnect(PlayerIndex, slot, pszName,
                                                xuid, pszNetworkID, pszAddress,
                                                bFakePlayer);
}
}  // namespace events
