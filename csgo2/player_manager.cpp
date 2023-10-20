#include "player_manager.h"
namespace ExtendPlayerManager {
std::shared_mutex mutex_Table_PlayerSteamIdPlayerSlot;
std::unordered_map<uint64_t, _ExtendPlayerSetting> Table_PlayerSteamIdPlayerSlot;
//GPT error fixed;
auto SteamIDStringToUInt64(const std::string& steamID) -> uint64_t {
    std::size_t pos = steamID.find_last_of(":");
    if (pos != std::string::npos) {
        uint64_t x = std::stoull(steamID.substr(pos + 1));
        return 76561197960265728 + x;
    }
    return -1;
}
auto AddSteamIdToPlayerSteamIdWithNameTable(uint64_t SteamId,
                                            uint64_t PlayerSlot) -> void {
    std::unique_lock<std::shared_mutex> lock(
        mutex_Table_PlayerSteamIdPlayerSlot);
    Table_PlayerSteamIdPlayerSlot.insert(std::make_pair(SteamId, _ExtendPlayerSetting{.playerSlot = PlayerSlot }));
}
auto UpdatePlayerSettingBySteamId(uint64_t SteamId, _ExtendPlayerSetting setting) -> void {
    std::shared_lock<std::shared_mutex> lock(
        mutex_Table_PlayerSteamIdPlayerSlot);

    auto it = Table_PlayerSteamIdPlayerSlot.find(SteamId);
    if (it != Table_PlayerSteamIdPlayerSlot.end()) {
        it->second.bloodSetting = setting.bloodSetting;
        it->second.weaponSetting = setting.weaponSetting;
    }
}
auto GetPlayerSettingBySteamId(uint64_t SteamId) -> std::pair<bool, _ExtendPlayerSetting> {
    std::shared_lock<std::shared_mutex> lock(
        mutex_Table_PlayerSteamIdPlayerSlot);

    auto it = Table_PlayerSteamIdPlayerSlot.find(SteamId);
    if (it != Table_PlayerSteamIdPlayerSlot.end()) {
        return { true, it->second };
    }
    return { false, {} };
}
auto GetPlayerSlotBySteamId(uint64_t SteamId) -> uint64_t {
    std::shared_lock<std::shared_mutex> lock(
        mutex_Table_PlayerSteamIdPlayerSlot);

    auto it = Table_PlayerSteamIdPlayerSlot.find(SteamId);
    if (it != Table_PlayerSteamIdPlayerSlot.end()) {
        return it->second.playerSlot;
    }
    return -1;
}
auto GetPlayerSteamIdByPlayerSlot(uint64_t playerSlot) -> uint64_t {
    std::shared_lock<std::shared_mutex> lock(
        mutex_Table_PlayerSteamIdPlayerSlot);
    for (auto& [SteamId, PlayerSetting] : Table_PlayerSteamIdPlayerSlot) {
        if (PlayerSetting.playerSlot == playerSlot) {
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
    for (int i = 0; i <= global::MaxPlayers; ++i) {
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
