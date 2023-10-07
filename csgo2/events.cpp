#include "events.h"

namespace events {
auto OnRoundEndEvent(IGameEvent* event) -> void {
    /*
        "winner"	"byte"		// winner team/user i
        "reason"	"byte"		// reson why team won
        "message"	"string"	// end round message
    */

    UnkGameEventStruct_t winnerNameParams{0};
    UnkGameEventStruct_t reasonNameParams{0};
    UnkGameEventStruct_t messageNameParams{0};

    static const auto winnerStr = "winner";
    static const auto reasonStr = "reason";
    static const auto messageStr = "message";

    winnerNameParams.m_Unk = Offset::FnServerHashFunction(
        winnerStr, sizeof winnerStr, SERVER_HASH_FUCNTION_KEY);
    winnerNameParams.m_Key = winnerStr;

    reasonNameParams.m_Unk = Offset::FnServerHashFunction(
        reasonStr, sizeof reasonStr, SERVER_HASH_FUCNTION_KEY);
    reasonNameParams.m_Key = reasonStr;

    messageNameParams.m_Unk = Offset::FnServerHashFunction(
        messageStr, sizeof messageStr, SERVER_HASH_FUCNTION_KEY);
    messageNameParams.m_Key = messageStr;

    const auto message = event->GetString(&messageNameParams);
    const auto winner = event->GetInt(&winnerNameParams);
    const auto reason = event->GetInt(&reasonNameParams);

    ScriptCallBacks::luaCall_onRoundEnd(winner, reason, message);
}
auto OnRoundStartEvent(IGameEvent* event) -> void {
    UnkGameEventStruct_t timelimitNameParams{0};
    static const auto timelimitStr = "timelimit";
    timelimitNameParams.m_Unk = Offset::FnServerHashFunction(
        timelimitStr, sizeof timelimitStr, SERVER_HASH_FUCNTION_KEY);
    timelimitNameParams.m_Key = timelimitStr;
    const auto timelimit = event->GetInt(&timelimitNameParams);
    ScriptCallBacks::luaCall_onRoundStart(timelimit);
}
auto OnPlayerSpawnEvent(IGameEvent* event) -> void {
    UnkGameEventStruct_t userIdNameParams{"userid"};
    const auto playerPawn = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(&userIdNameParams));
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
    UnkGameEventStruct_t userIdNameParams{"userid"};
    UnkGameEventStruct_t attackerNameParams{"attacker"};
    UnkGameEventStruct_t headshotNameParams{0};
    static const auto headShotStr = "headshot";
    headshotNameParams.m_Unk = Offset::FnServerHashFunction(
        headShotStr, sizeof headShotStr, SERVER_HASH_FUCNTION_KEY);
    headshotNameParams.m_Key = headShotStr;
    const auto victimPawn = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(&userIdNameParams));
    const auto attackerPawn = reinterpret_cast<CCSPlayerPawn*>(
        event->GetPlayerPawn(&attackerNameParams));
    const auto isHeadShot = event->GetBool(&headshotNameParams);
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
    // printf("player[%p] %s kill[%p] %llu\n", attacker,
    // &attacker->m_iszPlayerName(), victim,  &victim->m_steamID());
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
