#pragma once
#include "head.h"
enum class _ExtendPlayerSetting_Blood
{
    kNone,
    kDisableBloodWorldDecal,
    kDisableBloodEffectDispatch,
    kDisableBloodWorldDecalAndEffectDispatch,
    kMax
};
enum class _ExtendPlayerSetting_Weapon
{
    kNone,
    kDisablebulletHole,
    kMax
};
struct _ExtendPlayerSetting {
    uint64_t playerSlot;
    _ExtendPlayerSetting_Blood bloodSetting;
    _ExtendPlayerSetting_Weapon weaponSetting;
};
namespace ExtendPlayerManager {
auto AddSteamIdToPlayerSteamIdWithNameTable(uint64_t SteamId,
                                            uint64_t PlayerSlot) -> void;
auto GetPlayerSlotBySteamId(uint64_t SteamId) -> uint64_t;
auto SteamIDStringToUInt64(const std::string& steamID) -> uint64_t;
auto RemovePlayerSlotBySteamId(uint64_t SteamId) -> void;
auto GetPlayerByPlayerSlot(uint64_t playerSlot) -> CCSPlayerController*;
auto UpdatePlayerSettingBySteamId(uint64_t SteamId, _ExtendPlayerSetting setting) -> void;
auto GetPlayerSettingBySteamId(uint64_t SteamId)->std::pair<bool, _ExtendPlayerSetting>;
};  // namespace ExtendPlayerManager
