#include "player_manager.h"
namespace PlayerManager {
std::shared_mutex mutex_PlayerNameList;
std::unordered_map<uint32_t, std::string> PlayerNameList;
auto AddPlayerNameToPlayerNameList(const CPlayerSlot PlayerSlot,
                                   const char* PlayerName) -> void {
    std::unique_lock lock(mutex_PlayerNameList);
    PlayerNameList[PlayerSlot.Get()] = PlayerName;
    LOG("%s PlayerNameList[%d] = %s \n", __FUNCTION__, PlayerSlot.Get(),
        PlayerName);
}
auto RemovePlayerNameFromPlayerNameList(const CPlayerSlot PlayerSlot,
                                        const char* PlayerName) -> void {
    std::unique_lock lock(mutex_PlayerNameList);
    PlayerNameList.erase(PlayerSlot.Get());
    LOG("%s PlayerNameList[%d] = %s \n", __FUNCTION__, PlayerSlot.Get(),
        PlayerName);
}
auto GetPlayerNameByPlayerSlot(const CPlayerSlot PlayerSlot) -> std::string {
    std::shared_lock lock(mutex_PlayerNameList);
    auto index = PlayerSlot.Get();
    auto name = PlayerNameList[index];
    LOG("get player name: %d %s \n", index, name.c_str());
    return name;
}
};  // namespace PlayerManager
