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
    std::string weaponBaseName;
    int weaponType;
    int weaponIndex;
};
namespace ScriptApis {
auto RunTickCallBack(_GameTickRunTime* timer) -> void {
    LOG("excute tick function: %d %d \n", timer->m_iLuaCallBackFn,
        timer->m_iParamIndex);
    lua_rawgeti(timer->m_luaVm, LUA_REGISTRYINDEX, timer->m_iLuaCallBackFn);
    lua_rawgeti(timer->m_luaVm, LUA_REGISTRYINDEX, timer->m_iParamIndex);
    lua_pcall(timer->m_luaVm, 1, 0, 0);
    luaL_unref(timer->m_luaVm, LUA_REGISTRYINDEX, timer->m_iLuaCallBackFn);
    luaL_unref(timer->m_luaVm, LUA_REGISTRYINDEX, timer->m_iParamIndex);
}
auto TimerCallBack(_GameTimer* timer) -> void {
    // LOG("excute timer: %d %d m_bRepeat: %d\n", timer->m_iLuaCallBackFn,
    //     timer->m_iParamIndex, timer->m_bRepeat);
    lua_rawgeti(timer->m_luaVm, LUA_REGISTRYINDEX, timer->m_iLuaCallBackFn);
    lua_rawgeti(timer->m_luaVm, LUA_REGISTRYINDEX, timer->m_iParamIndex);
    lua_pcall(timer->m_luaVm, 1, 0, 0);
    if (timer->m_bRepeat == false) {
        luaL_unref(timer->m_luaVm, LUA_REGISTRYINDEX, timer->m_iLuaCallBackFn);
        luaL_unref(timer->m_luaVm, LUA_REGISTRYINDEX, timer->m_iParamIndex);
    }
}
auto ExcutePlayerAction(int playerIndex,
                        std::function<void(CCSPlayerController*)> function)
    -> void {
    CGameEntitySystem* EntitySystem = global::EntitySystem;
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
        function(playerController);
    } while (false);
}
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
        std::unique_lock lock(ScriptCallBacks::mutex_callbackList);
        ScriptCallBacks::callbackList[luaVm][callbackType] =
            luaL_ref(luaVm, LUA_REGISTRYINDEX);

        LOG("luaApi_ListenToGameEvent eventName:%s callback added\n",
            eventName);
    } while (false);

    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_SetPlayerCurrentWeaponAmmo(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto playerAmmoNum = lua_tointeger(luaVm, 2);
    const auto playerReserveAmmoNum = lua_tointeger(luaVm, 3);
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        const auto weaponServices = playerController->m_hPawn()
                                        .Get<CCSPlayerPawn>()
                                        ->m_pWeaponServices();
        if (weaponServices == nullptr) {
            return;
        }
        const auto activeWeapon =
            weaponServices->m_hActiveWeapon().Get<CBasePlayerWeapon>();
        if (activeWeapon == nullptr) {
            return;
        }
        if (playerAmmoNum != -1) {
            activeWeapon->m_iClip1(playerAmmoNum);
        }
        if (playerReserveAmmoNum != -1) {
            activeWeapon->m_pReserveAmmo(playerReserveAmmoNum);
        }
    });
    lua_pop(luaVm, 3);
    return 0;
}
//
auto luaApi_RespawnPlayer(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    int playerArmorValue = 0;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        const auto playerPawn =
            playerController->m_hPawn().Get<CCSPlayerPawn>();
        if (playerPawn == nullptr) {
            return;
        }
        Offset::InterFaces::CCSGameRulesInterFace->PlayerRespawn(playerPawn);
    });
    return 0;
}
auto luaApi_RespawnPlayerInDeathMatch(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    int playerArmorValue = 0;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        const auto playerPawn =
            playerController->m_hPawn().Get<CCSPlayerPawn>();
        if (playerPawn == nullptr) {
            return;
        }
        Offset::FnRespawnPlayerInDeathMatch(playerPawn);
    });
    return 0;
}
auto luaApi_SetPlayerArmorValue(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto playerArmorValue = lua_tointeger(luaVm, 2);
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        auto playerPawn = playerController->m_hPawn().Get<CCSPlayerPawn>();
        playerPawn->m_ArmorValue(playerArmorValue);
    });
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_GetPlayerArmorValue(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    int playerArmorValue = 0;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        auto playerPawn = playerController->m_hPawn().Get<CCSPlayerPawn>();
        playerArmorValue = playerPawn->m_ArmorValue();
    });
    lua_pop(luaVm, 1);
    lua_pushinteger(luaVm, playerArmorValue);
    return 1;
}
auto luaApi_GetPlayerHealth(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    int playerHealth = 0;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        auto playerPawn = playerController->m_hPawn().Get<CCSPlayerPawnBase>();
        playerHealth = playerPawn->m_iHealth();
    });
    lua_pop(luaVm, 1);
    lua_pushinteger(luaVm, playerHealth);
    return 1;
}
auto luaApi_SetPlayerHealth(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto playerHealth = lua_tointeger(luaVm, 2);
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        playerController->m_hPawn().Get()->m_iHealth(playerHealth);
    });
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_GetPlayerWeaponInfo(lua_State* luaVm) -> _luaApi_WeaponInfo {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto weaponIndex = lua_tointeger(luaVm, 2);
    _luaApi_WeaponInfo info{0};
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        const auto weaponServices = playerController->m_hPawn()
                                        .Get<CCSPlayerPawn>()
                                        ->m_pWeaponServices();
        do {
            if (weaponServices == nullptr) {
                break;
            }
            const auto weapons = weaponServices->m_hMyWeapons();

            // Create a new table on the Lua stack
            lua_newtable(luaVm);

            int index = 1;  // Lua tables start at index 1
            CBasePlayerWeapon* activeWeapon = nullptr;
            for (CHandle* handle = weapons.begin(); handle < weapons.end();
                 ++handle) {
                const auto weapon = handle->Get();
                if (weapon == nullptr) {
                    continue;
                }
                const auto _weaponIndex =
                    weapon->GetRefEHandle().GetEntryIndex();
                if (weaponIndex != _weaponIndex) {
                    continue;
                }
                activeWeapon = handle->Get<CBasePlayerWeapon>();
                break;
            }
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
            const char* checkWeaponName =
                Offset::InterFaces::ILocalize->FindSafe(
                    itemStaticData->m_pszItemBaseName);
            if (checkWeaponName == nullptr || strlen(checkWeaponName) < 1) {
                break;
            }
            info.isSuccess = true;
            info.Ammo = activeWeapon->m_iClip1();
            info.ReserveAmmo = activeWeapon->m_pReserveAmmo();
            info.weaponName = itemStaticData->GetSimpleWeaponName();
            info.weaponBaseName = itemStaticData->m_pszItemBaseName;
            info.weaponType =
                static_cast<int>(itemStaticData->IsKnife(false)
                                     ? _luaApi_WeaponType::kKnife
                                     : (itemStaticData->IsWeapon()
                                            ? _luaApi_WeaponType::kGun
                                            : _luaApi_WeaponType::kOther));
            info.weaponIndex = weaponIndex;
            // LOG("luaApi_GetPlayerWeaponInfo: %s %s %s %d \n",
            // info.weaponName.c_str(), info.weaponBaseName.c_str(),
            // checkWeaponName, weaponIndex);
        } while (false);
    });
    return info;
}
auto luaApi_GetPlayerCurrentWeaponInfo(lua_State* luaVm) -> _luaApi_WeaponInfo {
    // param: playerIndex:int
    const auto playerIndex = lua_tointeger(luaVm, 1);
    _luaApi_WeaponInfo info{0};
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        do {
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
            const char* checkWeaponName =
                Offset::InterFaces::ILocalize->FindSafe(
                    itemStaticData->m_pszItemBaseName);
            if (checkWeaponName == nullptr || strlen(checkWeaponName) < 1) {
                break;
            }
            info.isSuccess = true;
            info.Ammo = activeWeapon->m_iClip1();
            info.ReserveAmmo = activeWeapon->m_pReserveAmmo();
            info.weaponName = itemStaticData->GetSimpleWeaponName();
            info.weaponBaseName = itemStaticData->m_pszItemBaseName;

            info.weaponType =
                static_cast<int>(itemStaticData->IsKnife(false)
                                     ? _luaApi_WeaponType::kKnife
                                     : (itemStaticData->IsWeapon()
                                            ? _luaApi_WeaponType::kGun
                                            : _luaApi_WeaponType::kOther));
            info.weaponIndex = weaponServices->m_hActiveWeapon()
                                   .Get()
                                   ->GetRefEHandle()
                                   .GetEntryIndex();
        } while (false);
    });

    return info;
}
auto luaApi_CreateTickRunFunction(lua_State* luaVm) -> int {
    // params:lua_table, callback:function
    if (!lua_istable(luaVm, 1)) {
        luaL_error(luaVm, "Parameter 'params' must be a table");
        return 0;
    }
    lua_pushvalue(luaVm, 1);  // Duplicate the table for luaL_ref
    const auto params = luaL_ref(luaVm, LUA_REGISTRYINDEX);

    if (!lua_isfunction(luaVm, 2)) {
        luaL_error(luaVm, "Parameter 'callback' must be a function");
        return 0;
    }
    lua_pushvalue(luaVm, 2);  // Duplicate the function for luaL_ref
    const auto callback = luaL_ref(luaVm, LUA_REGISTRYINDEX);
    GameTickRunTime::AddTickFunction(new _GameTickRunTime{
        .m_luaVm = luaVm,
        .m_iParamIndex = params,
        .m_iLuaCallBackFn = callback,
    });
    return 0;
}
auto luaApi_CreateTimer(lua_State* luaVm) -> int {
    // param: time:float,repeat:bool,preservermapchange:bool,params:lua_table,
    // callback:function
    const auto time = luaL_checknumber(luaVm, 1);
    const auto repeat = lua_toboolean(luaVm, 2);
    const auto preserveMapChange = lua_toboolean(luaVm, 3);

    if (!lua_istable(luaVm, 4)) {
        luaL_error(luaVm, "Parameter 'params' must be a table");
        return 0;
    }
    lua_pushvalue(luaVm, 4);  // Duplicate the table for luaL_ref
    const auto params = luaL_ref(luaVm, LUA_REGISTRYINDEX);

    if (!lua_isfunction(luaVm, 5)) {
        luaL_error(luaVm, "Parameter 'callback' must be a function");
        return 0;
    }
    lua_pushvalue(luaVm, 5);  // Duplicate the function for luaL_ref
    const auto callback = luaL_ref(luaVm, LUA_REGISTRYINDEX);

    auto timerHandle = GameTimer::AddTimer(
        new _GameTimer{.m_flTime = (float)time,
                       .m_bRepeat = (bool)repeat,
                       .m_bPreserveMapChange = (bool)preserveMapChange,
                       .m_luaVm = luaVm,
                       .m_iParamIndex = params,
                       .m_iLuaCallBackFn = callback});

    lua_pushinteger(luaVm, timerHandle);
    return 1;
}
auto luaApi_CheckPlayerIsAlive(lua_State* luaVm) -> int {
    // param: playerIndex:int
    const auto playerIndex = lua_tointeger(luaVm, 1);
    auto isAlive = false;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        isAlive = playerController->m_bPawnIsAlive();
    });
    lua_pop(luaVm, 1);
    lua_pushboolean(luaVm, isAlive);
    return 1;
}
auto luaApi_GetPlayerTeam(lua_State* luaVm) -> int {
    // param: playerIndex:int
    const auto playerIndex = lua_tointeger(luaVm, 1);
    auto team = 0;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        team = playerController->m_iTeamNum();
    });
    lua_pop(luaVm, 1);
    lua_pushinteger(luaVm, team);
    return 1;
}
auto luaApi_SetPlayerTeam(lua_State* luaVm) -> int {
    // param: playerIndex:int, team:int
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto team = lua_tointeger(luaVm, 2);
    auto isSuccess = false;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        playerController->m_iTeamNum(team);
        isSuccess = true;
    });
    lua_pop(luaVm, 2);
    lua_pushboolean(luaVm, isSuccess);
    return 1;
}
auto luaApi_CheckPlayerIsInServer(lua_State* luaVm) -> int {
    // param: playerIndex:int
    const auto playerIndex = lua_tointeger(luaVm, 1);
    auto isInServer = false;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        isInServer = true;
    });

    lua_pop(luaVm, 1);
    lua_pushboolean(luaVm, isInServer);
    return 1;
}
auto luaApi_GivePlayerWeapon(lua_State* luaVm) -> int {
    // param: playerIndex:int, itemClass:string
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto weaponName = lua_tostring(luaVm, 2);
    auto isSuccess = false;
    if (weaponName != nullptr) {
        ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
            if (playerController->m_bPawnIsAlive() == false) {
               return;
            }
            isSuccess = GameWeapons::ParseWeaponCommand(playerController, weaponName);
        });
    }
    lua_pop(luaVm, 2);
    lua_pushboolean(luaVm, isSuccess);
    return 1;
}
auto luApi_GetPlayerAllWeaponIndex(lua_State* luaVm) -> int {
    // param: playerIndex:int
    const auto playerIndex = lua_tointeger(luaVm, 1);

    // Create a new table on the Lua stack
    lua_newtable(luaVm);

    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        do {
            const auto weaponServices = playerController->m_hPawn()
                                            .Get<CCSPlayerPawn>()
                                            ->m_pWeaponServices();
            if (weaponServices == nullptr) {
                break;
            }
            const auto weapons = weaponServices->m_hMyWeapons();
            int index = 1;  // Lua tables start at index 1
            for (CHandle* handle = weapons.begin(); handle < weapons.end();
                 ++handle) {
                const auto weapon = handle->Get();
                if (weapon == nullptr) {
                    continue;
                }
                const auto activeWeapon = handle->Get<CBasePlayerWeapon>();
                const auto attributeManager =
                    activeWeapon->m_AttributeManager();
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
                const char* checkWeaponName =
                    Offset::InterFaces::ILocalize->FindSafe(
                        itemStaticData->m_pszItemBaseName);
                if (checkWeaponName == nullptr || strlen(checkWeaponName) < 1) {
                    break;
                }
                // printf("weapon name: %s \n",
                // itemStaticData->GetSimpleWeaponName());
                const auto weaponIndex =
                    weapon->GetRefEHandle().GetEntryIndex();

                // Push the value onto the stack
                lua_pushinteger(luaVm, weaponIndex);

                // Set the table at index position
                lua_rawseti(luaVm, -2, index++);
            }
        } while (false);
    });

    // Return the table regardless of success or failure
    return 1;
}
auto luaApi_MakePlayerWeaponDrop(lua_State* luaVm) -> int {
    // param: playerIndex:int, itemClass:string
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto weaponIndex = lua_tointeger(luaVm, 2);
    auto isSuccess = false;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        do {
            const auto weaponServices = playerController->m_hPawn()
                                            .Get<CCSPlayerPawn>()
                                            ->m_pWeaponServices();
            if (weaponServices == nullptr) {
                break;
            }
            const auto weapons = weaponServices->m_hMyWeapons();
            CBasePlayerWeapon* activeWeapon = 0;
            for (CHandle* handle = weapons.begin(); handle < weapons.end();
                 ++handle) {
                if (handle->GetEntryIndex() != weaponIndex) {
                    continue;
                }
                const auto weapon = handle->Get<CBasePlayerWeapon>();
                if (weapon == nullptr) {
                    break;
                }
                activeWeapon = weapon;
                break;
            }
            if (activeWeapon == nullptr) {
                break;
            }
            weaponServices->RemoveWeapon(activeWeapon);
            isSuccess = true;
        } while (false);
    });
    lua_pop(luaVm, 2);
    lua_pushboolean(luaVm, isSuccess);
    return 1;
}
auto luaApi_MakePlayerCurrentWeaponDrop(lua_State* luaVm) -> int {
    // param: playerIndex:int, itemClass:string
    const auto playerIndex = lua_tointeger(luaVm, 1);
    auto isSuccess = false;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        do {
            const auto weaponServices = playerController->m_hPawn()
                                            .Get<CCSPlayerPawn>()
                                            ->m_pWeaponServices();
            if (weaponServices == nullptr) {
                break;
            }
            const auto activeWeapon =
                weaponServices->m_hActiveWeapon().Get<CBasePlayerWeapon>();
            if (activeWeapon == nullptr) {
                return;
            }
            weaponServices->RemoveWeapon(activeWeapon);
            isSuccess = true;
        } while (false);
    });
    lua_pop(luaVm, 1);
    lua_pushboolean(luaVm, isSuccess);
    return 1;
}
auto luaApi_RemovePlayerWeapon(lua_State* luaVm) -> int {
    // param: playerIndex:int, itemClass:string
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto weaponIndex = lua_tointeger(luaVm, 2);
    auto isSuccess = false;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        do {
            const auto weaponServices = playerController->m_hPawn()
                                            .Get<CCSPlayerPawn>()
                                            ->m_pWeaponServices();
            if (weaponServices == nullptr) {
                break;
            }
            const auto weapons = weaponServices->m_hMyWeapons();
            CBasePlayerWeapon* activeWeapon = 0;
            for (CHandle* handle = weapons.begin(); handle < weapons.end();
                 ++handle) {
                if (handle->GetEntryIndex() != weaponIndex) {
                    continue;
                }
                const auto weapon = handle->Get<CBasePlayerWeapon>();
                if (weapon == nullptr) {
                    continue;
                }
                activeWeapon = weapon;
                break;
            }
            if (activeWeapon == nullptr) {
                break;
            }
            weaponServices->RemoveWeapon(activeWeapon);
            Offset::FnEntityRemove(global::EntitySystem, activeWeapon, nullptr,
                                   -1);
            isSuccess = true;
        } while (false);
    });

    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_SendToPlayerChat(lua_State* luaVm) -> int {
    // param: playerIndex:int, message:string
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto hudType = lua_tointeger(luaVm, 2);
    const auto message = lua_tostring(luaVm, 3);
    if (hudType >= static_cast<int>(_HubType::kMax) ||
        hudType < static_cast<int>(_HubType::kNotify)) {
        lua_pop(luaVm, 3);
        return 0;
    }
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        SdkTools::SentChatToClient(playerController,
                                   static_cast<_HubType>(hudType), message);
    });
    lua_pop(luaVm, 3);
    return 0;
}
auto luaApi_SentToAllPlayerChat(lua_State* luaVm) -> int {
    // param: playerIndex:int, message:string
    const auto message = lua_tostring(luaVm, 1);
    const auto hudType = lua_tointeger(luaVm, 2);
    if (hudType >= static_cast<int>(_HubType::kMax) ||
        hudType < static_cast<int>(_HubType::kNotify)) {
        lua_pop(luaVm, 3);
        return 0;
    }
    SdkTools::SendConsoleChat(static_cast<_HubType>(hudType), message);
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_ChangePlayeriAccount(lua_State* luaVm) -> int {
    // param: playerIndex:int, x:float, y:float, z:float
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto number = lua_tonumber(luaVm, 2);
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        playerController->m_pInGameMoneyServices()->m_iAccount(number);
    });
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_GetPlayeriAccount(lua_State* luaVm) -> int {
    // param: playerIndex:int, x:float, y:float, z:float
    const auto playerIndex = lua_tointeger(luaVm, 1);
    auto number = 0;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        number = playerController->m_pInGameMoneyServices()->m_iAccount();
    });
    lua_pop(luaVm, 1);
    lua_pushinteger(luaVm, number);
    return 1;
}
auto luaApi_GetPlayerVecBaseVelocity(lua_State* luaVm) -> int {
    // param: playerIndex:int, x:float, y:float, z:float
    const auto playerIndex = lua_tointeger(luaVm, 1);
    Vector number;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        number = playerController->m_hPawn()
                     .Get<CCSPlayerPawnBase>()
                     ->m_vecBaseVelocity();
    });
    lua_pop(luaVm, 1);
    lua_pushnumber(luaVm, number.x);
    lua_pushnumber(luaVm, number.y);
    lua_pushnumber(luaVm, number.z);
    return 3;
}
auto luaApi_ChangePlayerVecBaseVelocity(lua_State* luaVm) -> int {
    // param: playerIndex:int, x:float, y:float, z:float
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto x = lua_tonumber(luaVm, 2);
    const auto y = lua_tonumber(luaVm, 3);
    const auto z = lua_tonumber(luaVm, 4);
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        playerController->m_hPawn().Get<CCSPlayerPawnBase>()->m_vecBaseVelocity(
            Vector(x, y, z));
    });
    lua_pop(luaVm, 4);
    return 0;
}

auto luaApi_SetPlayerGlowColor(lua_State* luaVm) -> int {
    // param: playerIndex:int, r:float, g:float, b:float, a:float
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto r = lua_tonumber(luaVm, 2);
    const auto g = lua_tonumber(luaVm, 3);
    const auto b = lua_tonumber(luaVm, 4);
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        playerController->m_hPawn()
            .Get<CBaseModelEntity>()
            ->m_Glow()
            .m_glowColorOverride(Color(r, g, b, 230));
    });
    lua_pop(luaVm, 4);
    return 0;
}
auto luaApi_SetPlayerGlowEnable(lua_State* luaVm) -> int {
    // param: playerIndex:int, isEnable:int
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto isEnable = lua_toboolean(luaVm, 2);
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        LOG("glow set %d to %d \n",
            playerController->m_hPawn()
                .Get<CBaseModelEntity>()
                ->m_Glow()
                .m_bGlowing(),
            isEnable);
        playerController->m_hPawn()
            .Get<CBaseModelEntity>()
            ->m_Glow()
            .m_bGlowing(isEnable);
        playerController->m_hPawn()
            .Get<CBaseModelEntity>()
            ->m_Glow()
            .m_iGlowType(3);
        playerController->m_hPawn()
            .Get<CBaseModelEntity>()
            ->m_Glow()
            .m_glowColorOverride(Color(201, 0, 118, 230));
    });
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_RunServerCommand(lua_State* luaVm) -> int {
    const auto command = lua_tostring(luaVm, 1);
    Offset::InterFaces::IVEngineServer->ServerCommand(command);
    lua_pop(luaVm, 1);
    return 0;
}
auto luaApi_RunClientCommand(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto command = lua_tostring(luaVm, 2);

    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        Offset::InterFaces::IVEngineServer->ClientCommand(
            EntityIndex_to_PlayerSlot(playerIndex), command);
    });
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_SetPlayerName(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto playerName = lua_tostring(luaVm, 2);

    if (playerName != nullptr) {
        ExcutePlayerAction(
            playerIndex, [&](CCSPlayerController* playerController) {
                Offset::FnPlayerChangeName(playerController->m_hPawn().Get(),
                                           (char*)playerName);
            });
    }
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_SetPlayerNameSlient(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto playerName = lua_tostring(luaVm, 2);

    if (playerName != nullptr) {
        ExcutePlayerAction(
            playerIndex, [&](CCSPlayerController* playerController) {
                const auto playerNameAddress =
                    &playerController->m_iszPlayerName();
                const auto playerNameLength = min(strlen(playerName) + 1, 127);
                memcpy(playerNameAddress, playerName, playerNameLength);
            });
    }
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_GetPlayerName(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    std::string playerName;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        playerName = &playerController->m_iszPlayerName();
    });
    lua_pop(luaVm, 1);
    lua_pushstring(luaVm, playerName.c_str());
    return 1;
}
auto luaApi_GetPlayerSteamId(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    std::string steamid;
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        steamid = std::to_string(playerController->m_steamID());
    });
    lua_pop(luaVm, 1);
    lua_pushstring(luaVm, steamid.c_str());
    return 1;
}
auto luaApi_KickPlayer(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto reason = lua_tostring(luaVm, 2);
    ExcutePlayerAction(playerIndex, [&](CCSPlayerController* playerController) {
        auto playerSlot = EntityIndex_to_PlayerSlot(playerIndex);
        if (playerSlot == -1) {
            return;
        }
        const auto theReason =
            "You have kicked by server , reason: " + std::string(reason);
        Offset::InterFaces::IVEngineServer->DisconnectClient(playerSlot, 39);
        SdkTools::SentChatToClient(playerController, _HubType::kTalk,
                                   theReason.c_str());
    });
    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_GetAllPlayerIndex(lua_State* luaVm) -> int {
    // param: playerIndex:int, style:int
    lua_newtable(luaVm);
    CGameEntitySystem* EntitySystem = global::EntitySystem;
    do {
        if (EntitySystem == nullptr) {
            break;
        }
        int index = 1;  // Lua tables start at index 1
        for (size_t i = 0; i < global::MaxPlayers; i++) {
            auto player = EntitySystem->GetBaseEntity(i);

            if (player == nullptr) {
                continue;
            }
            if (player->IsBasePlayerController() == false) {
                continue;
            }
            lua_pushinteger(luaVm, player->GetRefEHandle().GetEntryIndex());
            lua_rawseti(luaVm, -2, index++);
        }
    } while (false);
    return 1;
}
auto luaApi_HttpGet(lua_State* luaVm) -> int {
    // param: url:string header:string timeOut:int
    const auto url = lua_tostring(luaVm, 1);
    const auto header = lua_tostring(luaVm, 2);
    const auto timeOut = lua_tointeger(luaVm, 3);
    auto strHeader = std::string(header);
    std::string response;
    auto result = Server::HttpGet(url, response, strHeader, timeOut);
    lua_pushinteger(luaVm, result);
    lua_pushstring(luaVm, response.c_str());
    return 2;
}
auto luaApi_HttpPost(lua_State* luaVm) -> int {
    // param: url:string header:string postdata:string timeOut:int
    const auto url = lua_tostring(luaVm, 1);
    const auto header = lua_tostring(luaVm, 2);
    const auto postdata = lua_tostring(luaVm, 3);
    const auto timeOut = lua_tointeger(luaVm, 4);
    auto strHeader = std::string(header);
    std::string response;
    auto result = Server::HttpPost(url, strHeader, postdata, response, timeOut);
    lua_pushinteger(luaVm, result);
    lua_pushstring(luaVm, response.c_str());
    return 2;
}
auto luaApi_HttpAsyncPost(lua_State* luaVm) -> int {
    // param: url:string header:string postdata:string timeOut:int
    // metadata:string
    const auto url = lua_tostring(luaVm, 1);
    const auto header = lua_tostring(luaVm, 2);
    const auto postdata = lua_tostring(luaVm, 3);
    const auto theTimeOut = lua_tointeger(luaVm, 4);
    const auto metadata = lua_tostring(luaVm, 5);
    struct _ctx {
        std::shared_ptr<std::string> url;
        std::shared_ptr<std::string> header;
        std::shared_ptr<std::string> postdata;
        std::shared_ptr<std::string> metadata;
        long timeOut;
    };
    _ctx* ctx = new _ctx{
        .url = std::make_shared<std::string>(url),
        .header = std::make_shared<std::string>(header),
        .postdata = std::make_shared<std::string>(postdata),
        .metadata = std::make_shared<std::string>(metadata),
        .timeOut = (int)theTimeOut,
    };
    if (ctx) {
        CreateThread(
            nullptr, 0,
            [](void* ctx) -> DWORD {
                const auto theCtx = reinterpret_cast<_ctx*>(ctx);
                auto strHeader = std::string(*theCtx->header.get());
                std::string response;
                auto result = Server::HttpPost(*theCtx->url.get(), strHeader,
                                               *theCtx->postdata.get(),
                                               response, theCtx->timeOut);
                ScriptCallBacks::luaCall_onHttpRequest(
                    *theCtx->url.get(), *theCtx->metadata.get(),
                    response.c_str(), result);
                delete theCtx;
                return 0;
            },
            ctx, 0, nullptr);
    }
    lua_pop(luaVm, 5);
    return 0;
}
auto luaApi_HttpAsyncGet(lua_State* luaVm) -> int {
    // param: url:string header:string  timeOut:int
    // metadata:string
    const auto url = lua_tostring(luaVm, 1);
    const auto header = lua_tostring(luaVm, 2);
    const auto theTimeOut = lua_tointeger(luaVm, 3);
    const auto metadata = lua_tostring(luaVm, 4);
    struct _ctx {
        std::shared_ptr<std::string> url;
        std::shared_ptr<std::string> header;
        std::shared_ptr<std::string> metadata;
        long timeOut;
    };
    _ctx* ctx = new _ctx{
        .url = std::make_shared<std::string>(url),
        .header = std::make_shared<std::string>(header),
        .metadata = std::make_shared<std::string>(metadata),
        .timeOut = (long)theTimeOut,
    };
    if (ctx) {
        CreateThread(
            nullptr, 0,
            [](void* ctx) -> DWORD {
                const auto theCtx = reinterpret_cast<_ctx*>(ctx);
                std::string response;
                auto httpCode =
                    Server::HttpGet(*theCtx->url.get(), response,
                                    *theCtx->header.get(), theCtx->timeOut);
                ScriptCallBacks::luaCall_onHttpRequest(
                    *theCtx->url.get(), *theCtx->metadata.get(),
                    response.c_str(), httpCode);
                delete theCtx;
                return 0;
            },
            const_cast<_ctx*>(ctx), 0, nullptr);
    }
    lua_pop(luaVm, 4);
    return 0;
}
auto luaApi_GetConVarString(lua_State* luaVm) -> int {
    // param: convarObject:int
    const auto inputData = lua_tointeger(luaVm, 1);
    std::string value;
    if (inputData != NULL) {
        ConVarHandle theConvarHandle{};
        theConvarHandle.Set(inputData);
        if (theConvarHandle.IsValid()) {
            const auto convarData =
                Offset::InterFaces::IVEngineCvar->GetConVar(theConvarHandle);
            if (convarData) {
                const auto address = convarData->values;
                const auto valueData = reinterpret_cast<char*>(address);
                value = valueData ? std::string(valueData) : "";
            }
        }
    }
    lua_pop(luaVm, 1);
    lua_pushstring(luaVm, value.c_str());

    return 1;
}
auto luaApi_GetConVarInt(lua_State* luaVm) -> int {
    // param: convarObject:int
    const auto inputData = lua_tointeger(luaVm, 1);
    if (inputData) {
        ConVarHandle theConvarHandle{};
        theConvarHandle.Set(inputData);
        int value = -1;
        if (theConvarHandle.IsValid()) {
            const auto convarData =
                Offset::InterFaces::IVEngineCvar->GetConVar(theConvarHandle);
            value = convarData ? reinterpret_cast<int>(convarData->values) : -1;
        }
        lua_pop(luaVm, 1);
        lua_pushinteger(luaVm, value);
    }
    return 1;
}
auto luaApi_GetConVarObject(lua_State* luaVm) -> int {
    // param: name:string
    const auto name = lua_tostring(luaVm, 1);
    lua_pushnumber(luaVm,
                   Offset::InterFaces::IVEngineCvar->FindConVar(name).Get());
    lua_pop(luaVm, 1);

    return 1;
}
auto luaApi_SetPlayerWeaponEffectStatus(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto status = lua_tointeger(luaVm, 2);
    auto playerSlot = EntityIndex_to_PlayerSlot(playerIndex);
    do
    {
        if (status >= static_cast<int>(_ExtendPlayerSetting_Weapon::kMax)
            || status < static_cast<int>(_ExtendPlayerSetting_Weapon::kNone)) {
            break;
        }
        if (playerSlot == -1) {
            break;
        }
        auto player = ExtendPlayerManager::GetPlayerByPlayerSlot(playerSlot);
        if (player == nullptr) {
            break;
        }
        auto [isSuccess, playerSetting] = ExtendPlayerManager::GetPlayerSettingBySteamId(player->m_steamID());
        if (isSuccess == false) {
            LOG("can't get player setting, wtf bug?? \n");
            break;
        }
        ExtendPlayerManager::UpdatePlayerSettingBySteamId(player->m_steamID(), _ExtendPlayerSetting{
            .bloodSetting = playerSetting.bloodSetting,
            .weaponSetting = static_cast<_ExtendPlayerSetting_Weapon>(status)
            });

    } while (false);

    lua_pop(luaVm, 2);
    return 0;
}
auto luaApi_SetPlayerBloodStatus(lua_State* luaVm) -> int {
    const auto playerIndex = lua_tointeger(luaVm, 1);
    const auto status = lua_tointeger(luaVm, 2);
    auto playerSlot = EntityIndex_to_PlayerSlot(playerIndex);
    do
    {
        if (status >= static_cast<int>(_ExtendPlayerSetting_Blood::kMax) 
            || status < static_cast<int>(_ExtendPlayerSetting_Blood::kNone)) {
            break;
        }
        if (playerSlot == -1) {
            break;
        }
        auto player = ExtendPlayerManager::GetPlayerByPlayerSlot(playerSlot);
        if (player == nullptr) {
            break;
        }
        auto [isSuccess, playerSetting] = ExtendPlayerManager::GetPlayerSettingBySteamId(player->m_steamID());
        if (isSuccess == false) {
            LOG("can't get player setting, wtf bug?? \n");
            break;
        }
        ExtendPlayerManager::UpdatePlayerSettingBySteamId(player->m_steamID(), _ExtendPlayerSetting {
            .bloodSetting = static_cast<_ExtendPlayerSetting_Blood>(status),
            .weaponSetting = playerSetting.weaponSetting
        });

    } while (false);

    lua_pop(luaVm, 2);
    return 0;
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
    lua_register(luaVm, "luaApi_RespawnPlayerInDeathMatch",
                 luaApi_RespawnPlayerInDeathMatch);
    lua_register(luaVm, "luaApi_CreateTimer", luaApi_CreateTimer);
    lua_register(luaVm, "luaApi_CreateTickRunFunction",
                 luaApi_CreateTickRunFunction);
    lua_register(luaVm, "luaApi_CheckPlayerIsAlive", luaApi_CheckPlayerIsAlive);
    lua_register(luaVm, "luaApi_GetPlayerTeam", luaApi_GetPlayerTeam);
    lua_register(luaVm, "luaApi_SetPlayerTeam", luaApi_SetPlayerTeam);
    lua_register(luaVm, "luaApi_CheckPlayerIsInServer",
                 luaApi_CheckPlayerIsInServer);
    lua_register(luaVm, "luaApi_GivePlayerWeapon", luaApi_GivePlayerWeapon);
    lua_register(luaVm, "luApi_GetPlayerAllWeaponIndex",
                 luApi_GetPlayerAllWeaponIndex);
    lua_register(luaVm, "luaApi_RemovePlayerWeapon", luaApi_RemovePlayerWeapon);
    lua_register(luaVm, "luaApi_MakePlayerWeaponDrop",
                 luaApi_MakePlayerWeaponDrop);
    lua_register(luaVm, "luaApi_MakePlayerCurrentWeaponDrop",
                 luaApi_MakePlayerCurrentWeaponDrop);
    lua_register(luaVm, "luaApi_SendToPlayerChat", luaApi_SendToPlayerChat);
    lua_register(luaVm, "luaApi_SentToAllPlayerChat",
                 luaApi_SentToAllPlayerChat);
    lua_register(luaVm, "luaApi_ChangePlayerVecBaseVelocity",
                 luaApi_ChangePlayerVecBaseVelocity);
    lua_register(luaVm, "luaApi_GetPlayerVecBaseVelocity",
                 luaApi_GetPlayerVecBaseVelocity);
    lua_register(luaVm, "luaApi_ChangePlayeriAccount",
                 luaApi_ChangePlayeriAccount);
    lua_register(luaVm, "luaApi_GetPlayeriAccount", luaApi_GetPlayeriAccount);
    lua_register(luaVm, "luaApi_SetPlayerGlowEnable",
                 luaApi_SetPlayerGlowEnable);
    lua_register(luaVm, "luaApi_SetPlayerGlowColor", luaApi_SetPlayerGlowColor);
    lua_register(luaVm, "luaApi_GetAllPlayerIndex", luaApi_GetAllPlayerIndex);
    lua_register(luaVm, "luaApi_RunServerCommand", luaApi_RunServerCommand);
    lua_register(luaVm, "luaApi_KickPlayer", luaApi_KickPlayer);
    lua_register(luaVm, "luaApi_HttpGet", luaApi_HttpGet);
    lua_register(luaVm, "luaApi_HttpPost", luaApi_HttpPost);
    lua_register(luaVm, "luaApi_HttpAsyncGet", luaApi_HttpAsyncGet);
    lua_register(luaVm, "luaApi_HttpAsyncPost", luaApi_HttpAsyncPost);
    lua_register(luaVm, "luaApi_GetConVarObject", luaApi_GetConVarObject);

    lua_register(luaVm, "luaApi_GetConVarString", luaApi_GetConVarString);
    lua_register(luaVm, "luaApi_GetConVarInt", luaApi_GetConVarInt);
    lua_register(luaVm, "luaApi_GetPlayerSteamId", luaApi_GetPlayerSteamId);
    lua_register(luaVm, "luaApi_RunClientCommand", luaApi_RunClientCommand);
    lua_register(luaVm, "luaApi_SetPlayerName", luaApi_SetPlayerName);
    lua_register(luaVm, "luaApi_GetPlayerName", luaApi_GetPlayerName);
    lua_register(luaVm, "luaApi_SetPlayerNameSlient",
                 luaApi_SetPlayerNameSlient);
    //lua_register(luaVm, "luaApi_SetPlayerWeaponEffectStatus", luaApi_SetPlayerWeaponEffectStatus);
    lua_register(luaVm, "luaApi_SetPlayerBloodStatus", luaApi_SetPlayerBloodStatus);
    // lua_register(luaVm, "luaApi_TeleportPlayer", luaApi_TeleportPlayer);

    luabridge::getGlobalNamespace(luaVm)
        .beginClass<_luaApi_WeaponInfo>("WeaponInfo")
        .addConstructor<void (*)(void)>()
        .addData("isSuccess", &_luaApi_WeaponInfo::isSuccess)
        .addData("Ammo", &_luaApi_WeaponInfo::Ammo)
        .addData("ReserveAmmo", &_luaApi_WeaponInfo::ReserveAmmo)
        .addData("weaponName", &_luaApi_WeaponInfo::weaponName)
        .addData("weaponBaseName", &_luaApi_WeaponInfo::weaponBaseName)
        .addData("weaponType", &_luaApi_WeaponInfo::weaponType)
        .addData("weaponIndex", &_luaApi_WeaponInfo::weaponIndex)
        .endClass()
        .addFunction("luaApi_GetPlayerWeaponInfo", &luaApi_GetPlayerWeaponInfo);
    luabridge::getGlobalNamespace(luaVm)
        .beginClass<_luaApi_WeaponInfo>("WeaponInfo")
        .addConstructor<void (*)(void)>()
        .addData("isSuccess", &_luaApi_WeaponInfo::isSuccess)
        .addData("Ammo", &_luaApi_WeaponInfo::Ammo)
        .addData("ReserveAmmo", &_luaApi_WeaponInfo::ReserveAmmo)
        .addData("weaponName", &_luaApi_WeaponInfo::weaponName)
        .addData("weaponBaseName", &_luaApi_WeaponInfo::weaponBaseName)
        .addData("weaponType", &_luaApi_WeaponInfo::weaponType)
        .addData("weaponIndex", &_luaApi_WeaponInfo::weaponIndex)
        .endClass()
        .addFunction("luaApi_GetPlayerCurrentWeaponInfo",
                     &luaApi_GetPlayerCurrentWeaponInfo);
}
};  // namespace ScriptApis
