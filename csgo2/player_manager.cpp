#include "player_manager.h"
namespace ExtendPlayerManager {
std::shared_mutex mutex_Table_PlayerSteamIdPlayerSlot;

std::unordered_map<uint64_t, uint64_t> Table_PlayerSteamIdPlayerSlot;

auto SteamIDStringToUInt64(const std::string& steamID) -> uint64_t {
    std::istringstream iss(
        steamID.substr(3, steamID.size() - 4));  // 去掉"[U:"和"]"
    std::string tmp;
    uint32_t instance, account_id;

    // 读取 Account Instance
    std::getline(iss, tmp, ':');
    instance = std::stoi(tmp);

    // 读取 Account ID
    std::getline(iss, tmp);
    account_id = std::stoi(tmp);

    // 计算并返回结果
    return (uint64_t(account_id) << 1 | instance) + 76561197960265728ULL;
}

auto AddSteamIdToPlayerSteamIdWithNameTable(uint64_t SteamId,
                                            uint64_t PlayerSlot) -> void {
    std::unique_lock<std::shared_mutex> lock(
        mutex_Table_PlayerSteamIdPlayerSlot);
    Table_PlayerSteamIdPlayerSlot.insert(std::make_pair(SteamId, PlayerSlot));
}
auto GetPlayerSlotBySteamId(uint64_t SteamId) -> uint64_t {
    std::shared_lock<std::shared_mutex> lock(
        mutex_Table_PlayerSteamIdPlayerSlot);

    auto it = Table_PlayerSteamIdPlayerSlot.find(SteamId);
    if (it != Table_PlayerSteamIdPlayerSlot.end()) {
        return it->second;
    }
    return -1;
}
auto GetPlayerSteamIdByPlayerSlot(uint64_t playerSlot) -> uint64_t {
    std::shared_lock<std::shared_mutex> lock(
        mutex_Table_PlayerSteamIdPlayerSlot);
    for (auto& [SteamId, PlayerSlot] : Table_PlayerSteamIdPlayerSlot) {
        if (PlayerSlot == playerSlot) {
            return SteamId;
        }
    }
    return -1;
}
auto RemovePlayerSlotBySteamId(uint64_t SteamId) -> void {
    std::unique_lock<std::shared_mutex> lock(
        mutex_Table_PlayerSteamIdPlayerSlot);
    if (Table_PlayerSteamIdPlayerSlot.find(SteamId) !=
        Table_PlayerSteamIdPlayerSlot.end()) {
        Table_PlayerSteamIdPlayerSlot.erase(SteamId);
    }
}
auto GetPlayerByPlayerSlot(uint64_t playerSlot) -> CCSPlayerController* {
    auto PlayerSteamId = GetPlayerSteamIdByPlayerSlot(playerSlot);
    if (PlayerSteamId == -1) {
        return nullptr;
    }
    CGameEntitySystem* pEntitySystem = global::EntitySystem;
    if (!pEntitySystem) {
        return nullptr;
    }
    for (int i = 1; i <= global::MaxPlayers; ++i) {
        CBaseEntity* pEntity = pEntitySystem->GetBaseEntity(i);
        if (!pEntity) continue;
        if (pEntity->IsBasePlayerController()) {
            const auto player = reinterpret_cast<CCSPlayerController*>(pEntity);
            if (player->m_steamID() == PlayerSteamId) {
                return player;
            }
        }
    }
    return nullptr;
}
};  // namespace ExtendPlayerManager
