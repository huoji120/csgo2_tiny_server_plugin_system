#include "script_apis.h"
enum class _luaApi_WeaponType {
    kOther,
    kKnife,
    kGun,
};
struct _luaApi_WeaponInfo {
    bool isSuccess;
    int Ammo;
    int ReserveAmmo;
    std::string weaponName;
    int weaponType;
};
namespace ScriptApis {
// 返回是返回值数量,返回值内容要自己push到stack上
auto luaApi_ListenToGameEvent(lua_State* luaVm) -> int {
    const auto eventName = lua_tostring(luaVm, 1);
    do {
        if (eventName == nullptr) {
            LOG("luaApi_ListenToGameEvent eventName == nullptr || callbackName "
                "== "
                "nullptr\n");
            break;
        }
        if (!lua_isfunction(luaVm, 2)) {
            LOG("luaApi_ListenToGameEvent callback is not a function\n");
            break;
        }
        auto callbackType = ScriptCallBacks::CallBackNameToEnum(eventName);
        if (callbackType == ScriptCallBacks::_CallbackNames::kError) {
            LOG("luaApi_ListenToGameEvent unknown event name: %s\n", eventName);
            break;
        }
        // 将回调添加到映射中
        std::unique_lock lock(ScriptCallBacks::mutex_callbackList);
        ScriptCallBacks::callbackList[luaVm][callbackType] =
            luaL_ref(luaVm, LUA_REGISTRYINDEX);

        LOG("luaApi_ListenToGameEvent eventName:%s callback added\n",
            eventName);
    } while (false);

    lua_pop(luaVm, 2);  // 清理堆栈
    return 0;
}
auto luaApi_SetPlayerCurrentWeaponAmmo(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto playerAmmoNum = lua_tointeger(luaVm, 2);
    const auto playerReserveAmmoNum = lua_tointeger(luaVm, 3);

    CGameEntitySystem* EntitySystem = CGameEntitySystem::GetInstance();
    do {
        if (EntitySystem == nullptr || playerIndex == 0) {
            break;
        }
        auto player = EntitySystem->GetBaseEntity(playerIndex);
        if (player == nullptr) {
            break;
        }
        if (player->IsBasePlayerController() == false) {
            break;
        }
        auto playerController = reinterpret_cast<CCSPlayerController*>(player);
        const auto weaponServices = playerController->m_hPawn()
                                        .Get<CCSPlayerPawn>()
                                        ->m_pWeaponServices();
        if (weaponServices == nullptr) {
            break;
        }
        const auto activeWeapon =
            weaponServices->m_hActiveWeapon().Get<CBasePlayerWeapon>();
        if (activeWeapon == nullptr) {
            break;
        }
        if (playerAmmoNum != -1) {
            activeWeapon->m_iClip1(playerAmmoNum);
        }
        if (playerReserveAmmoNum != -1) {
            activeWeapon->m_pReserveAmmo(playerReserveAmmoNum);
        }
    } while (false);
    lua_pop(luaVm, 3);
    return 0;
}
auto luaApi_RespawnPlayer(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    int playerArmorValue = 0;
    CGameEntitySystem* EntitySystem = CGameEntitySystem::GetInstance();
    do {
        if (EntitySystem == nullptr || playerIndex == 0) {
            break;
        }
        auto player = EntitySystem->GetBaseEntity(playerIndex);
        if (player == nullptr) {
            break;
        }
        if (player->IsBasePlayerController() == false) {
            break;
        }
        auto playerController = reinterpret_cast<CCSPlayerController*>(player);
        auto playerPawn = playerController->m_hPawn().Get<CCSPlayerPawn>();
        playerPawn->m_bRespawning(false);
    } while (false);
    lua_pop(luaVm, 1);
    return 0;
}
auto luaApi_SetPlayerArmorValue(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto playerArmorValue = lua_tointeger(luaVm, 2);

    CGameEntitySystem* EntitySystem = CGameEntitySystem::GetInstance();
    do {
        if (EntitySystem == nullptr || playerIndex == 0) {
            break;
        }
        auto player = EntitySystem->GetBaseEntity(playerIndex);
        if (player == nullptr) {
            break;
        }
        if (player->IsBasePlayerController() == false) {
            break;
        }
        auto playerController = reinterpret_cast<CCSPlayerController*>(player);
        auto playerPawn = playerController->m_hPawn().Get<CCSPlayerPawn>();
        playerPawn->m_ArmorValue(playerArmorValue);
    } while (false);
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_GetPlayerArmorValue(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    int playerArmorValue = 0;

    CGameEntitySystem* EntitySystem = CGameEntitySystem::GetInstance();
    do {
        if (EntitySystem == nullptr || playerIndex == 0) {
            break;
        }
        auto player = EntitySystem->GetBaseEntity(playerIndex);
        if (player == nullptr) {
            break;
        }
        if (player->IsBasePlayerController() == false) {
            break;
        }
        auto playerController = reinterpret_cast<CCSPlayerController*>(player);
        auto playerPawn = playerController->m_hPawn().Get<CCSPlayerPawn>();
        playerArmorValue = playerPawn->m_ArmorValue();
    } while (false);
    lua_pop(luaVm, 1);
    lua_pushinteger(luaVm, playerArmorValue);
    return 1;
}
auto luaApi_GetPlayerHealth(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    int playerHealth = 0;

    CGameEntitySystem* EntitySystem = CGameEntitySystem::GetInstance();
    do {
        if (EntitySystem == nullptr || playerIndex == 0) {
            break;
        }
        auto player = EntitySystem->GetBaseEntity(playerIndex);
        if (player == nullptr) {
            break;
        }
        if (player->IsBasePlayerController() == false) {
            break;
        }
        auto playerController = reinterpret_cast<CCSPlayerController*>(player);
        playerHealth = playerController->m_iHealth();
    } while (false);
    lua_pop(luaVm, 1);
    lua_pushinteger(luaVm, playerHealth);
    return 1;
}
auto luaApi_SetPlayerHealth(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto playerHealth = lua_tointeger(luaVm, 2);

    CGameEntitySystem* EntitySystem = CGameEntitySystem::GetInstance();
    do {
        if (EntitySystem == nullptr || playerIndex == 0) {
            break;
        }
        LOG("luaApi_SetPlayerHealth :2 \n");

        auto player = EntitySystem->GetBaseEntity(playerIndex);
        if (player == nullptr) {
            break;
        }
        if (player->IsBasePlayerController() == false) {
            break;
        }
        LOG("luaApi_SetPlayerHealth :3 \n");

        auto playerController = reinterpret_cast<CCSPlayerController*>(player);
        playerController->m_iHealth(playerHealth);
    } while (false);
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_GetPlayerCurrentWeaponInfo(lua_State* luaVm) -> _luaApi_WeaponInfo {
    // param: playerIndex:int
    const auto playerIndex = lua_tointeger(luaVm, 1);
    _luaApi_WeaponInfo info{0};

    CGameEntitySystem* EntitySystem = CGameEntitySystem::GetInstance();
    do {
        if (EntitySystem == nullptr || playerIndex == 0) {
            break;
        }
        auto player = EntitySystem->GetBaseEntity(playerIndex);
        if (player == nullptr) {
            break;
        }
        if (player->IsBasePlayerController() == false) {
            break;
        }
        auto playerController = reinterpret_cast<CCSPlayerController*>(player);
        const auto weaponServices = playerController->m_hPawn()
                                        .Get<CCSPlayerPawn>()
                                        ->m_pWeaponServices();
        if (weaponServices == nullptr) {
            break;
        }
        const auto activeWeapon =
            weaponServices->m_hActiveWeapon().Get<CBasePlayerWeapon>();
        if (activeWeapon == nullptr) {
            break;
        }
        const auto attributeManager = activeWeapon->m_AttributeManager();
        if (activeWeapon == nullptr) {
            break;
        }
        const auto itemView = attributeManager->m_Item();
        if (itemView == nullptr) {
            break;
        }
        const auto itemStaticData = itemView->GetStaticData();
        if (itemView == nullptr) {
            break;
        }
        const char* checkWeaponName = Offset::InterFaces::ILocalize->FindSafe(
            itemStaticData->m_pszItemBaseName);
        if (checkWeaponName == nullptr || strlen(checkWeaponName) < 1) {
            break;
        }
        info.isSuccess = true;
        info.Ammo = activeWeapon->m_iClip1();
        info.ReserveAmmo = activeWeapon->m_pReserveAmmo();
        info.weaponName = itemStaticData->GetSimpleWeaponName();
        info.weaponType = static_cast<int>(
            itemStaticData->IsKnife(false)
                ? _luaApi_WeaponType::kKnife
                : (itemStaticData->IsWeapon() ? _luaApi_WeaponType::kGun
                                              : _luaApi_WeaponType::kOther));
    } while (false);

    return info;
}
auto initFunciton(lua_State* luaVm) -> void {
    lua_register(luaVm, "ListenToGameEvent", luaApi_ListenToGameEvent);
    lua_register(luaVm, "luaApi_SetPlayerCurrentWeaponAmmo",
                 luaApi_SetPlayerCurrentWeaponAmmo);
    lua_register(luaVm, "luaApi_GetPlayerHealth", luaApi_GetPlayerHealth);
    lua_register(luaVm, "luaApi_SetPlayerHealth", luaApi_SetPlayerHealth);
    lua_register(luaVm, "luaApi_GetPlayerArmorValue",
                 luaApi_GetPlayerArmorValue);
    lua_register(luaVm, "luaApi_SetPlayerArmorValue",
                 luaApi_SetPlayerArmorValue);
    lua_register(luaVm, "luaApi_RespawnPlayer", luaApi_RespawnPlayer);

    // 我不喜欢他
    luabridge::getGlobalNamespace(luaVm)
        .beginClass<_luaApi_WeaponInfo>("WeaponInfo")
        .addConstructor<void (*)(void)>()
        .addData("isSuccess", &_luaApi_WeaponInfo::isSuccess)
        .addData("Ammo", &_luaApi_WeaponInfo::Ammo)
        .addData("ReserveAmmo", &_luaApi_WeaponInfo::ReserveAmmo)
        .addData("weaponName", &_luaApi_WeaponInfo::weaponName)
        .addData("weaponType", &_luaApi_WeaponInfo::weaponType)
        .endClass()
        .addFunction("luaApi_GetPlayerCurrentWeaponInfo",
                     &luaApi_GetPlayerCurrentWeaponInfo);
}
};  // namespace ScriptApis
