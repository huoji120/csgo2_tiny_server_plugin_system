#include "native_sdk.h"

CBaseEntity* CHandle::GetBaseEntity() const {
    CGameEntitySystem* pEntitySystem = CGameEntitySystem::GetInstance();
    if (!pEntitySystem) return nullptr;

    return pEntitySystem->GetBaseEntity(GetEntryIndex());
}
CGameEntitySystem* CGameEntitySystem::GetInstance() {
    return Offset::InterFaces::GameResourceServiceServer->GetGameEntitySystem();
}

auto CSchemaSystemTypeScope::FindDeclaredClass(const char* pClass)
    -> SchemaClassInfoData_t* {
    SchemaClassInfoData_t* rv = nullptr;
    CALL_VIRTUAL(void, 2, this, &rv, pClass);
    return rv;
}

auto CSchemaSystem::FindTypeScopeForModule(const char* module)
    -> CSchemaSystemTypeScope* {
    return CALL_VIRTUAL(CSchemaSystemTypeScope*, 13, this, module, nullptr);
}

auto CBaseEntity::IsBasePlayerController() -> bool {
    return CALL_VIRTUAL(bool, 144, this);
}
auto CBaseEntity::SpawnClientEntity() -> void { CALL_VIRTUAL(void, 19, this); }
auto CBasePlayer::ForceRespawn() -> void {
    return CALL_VIRTUAL(void, 26, this);
}

auto CCSPlayerPawn::GetPlayerController() -> CCSPlayerController* {
    CGameEntitySystem* pEntitySystem = CGameEntitySystem::GetInstance();
    if (!pEntitySystem) {
        return nullptr;
    }
    for (int i = 1; i <= global::MaxPlayers; ++i) {
        CBaseEntity* pEntity = pEntitySystem->GetBaseEntity(i);
        if (!pEntity) continue;
        if (pEntity->IsBasePlayerController()) {
            const auto player = reinterpret_cast<CCSPlayerController*>(pEntity);
            if (player->m_hPawn().Get() == this) {
                // printf("Found Pawn Player: %d %s \n",
                // player->GetRefEHandle().GetEntryIndex(),
                // &player->m_iszPlayerName());
                return player;
            }
        }
    }
    return nullptr;
}

using SchemaKeyValueMap_t = CUtlMap<uint32_t, SchemaKey>;
using SchemaTableMap_t = CUtlMap<uint32_t, SchemaKeyValueMap_t*>;

static bool IsFieldNetworked(SchemaClassFieldData_t& field) {
    for (int i = 0; i < field.m_metadata_size; i++) {
        static auto networkEnabled = hash_32_fnv1a_const("MNetworkEnable");
        if (networkEnabled == hash_32_fnv1a_const(field.m_metadata[i].m_name))
            return true;
    }

    return false;
}

static bool InitSchemaFieldsForClass(SchemaTableMap_t* tableMap,
                                     const char* className, uint32_t classKey) {
    CSchemaSystemTypeScope* pType =
        Offset::InterFaces::SchemaSystem->FindTypeScopeForModule("server.dll");

    if (!pType) return false;

    SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className);

    if (!pClassInfo) {
        SchemaKeyValueMap_t* map =
            new SchemaKeyValueMap_t(0, 0, DefLessFunc(uint32_t));
        tableMap->Insert(classKey, map);

        LOG("InitSchemaFieldsForClass(): '%s' was not found!\n", className);
        return false;
    }

    short fieldsSize = pClassInfo->GetFieldsSize();
    SchemaClassFieldData_t* pFields = pClassInfo->GetFields();

    SchemaKeyValueMap_t* keyValueMap =
        new SchemaKeyValueMap_t(0, 0, DefLessFunc(uint32_t));
    keyValueMap->EnsureCapacity(fieldsSize);
    tableMap->Insert(classKey, keyValueMap);

    for (int i = 0; i < fieldsSize; ++i) {
        SchemaClassFieldData_t& field = pFields[i];
        // LOG("%s::%s found at -> 0x%X - %llx\n", className, field.m_name,
        // field.m_offset, &field);

        keyValueMap->Insert(hash_32_fnv1a_const(field.m_name),
                            {field.m_offset, IsFieldNetworked(field)});
    }

    return true;
}

int16_t schema::FindChainOffset(const char* className) {
    CSchemaSystemTypeScope* pType =
        Offset::InterFaces::SchemaSystem->FindTypeScopeForModule("server.dll");

    if (!pType) return false;

    SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className);

    do {
        SchemaClassFieldData_t* pFields = pClassInfo->GetFields();
        short fieldsSize = pClassInfo->GetFieldsSize();
        for (int i = 0; i < fieldsSize; ++i) {
            SchemaClassFieldData_t& field = pFields[i];

            if (strcmp(field.m_name, "__m_pChainEntity") == 0) {
                return field.m_offset;
            }
        }
    } while ((pClassInfo = pClassInfo->GetParent()) != nullptr);

    return 0;
}

SchemaKey schema::GetOffset(const char* className, uint32_t classKey,
                            const char* memberName, uint32_t memberKey) {
    static SchemaTableMap_t schemaTableMap(0, 0, DefLessFunc(uint32_t));
    int16_t tableMapIndex = schemaTableMap.Find(classKey);
    if (!schemaTableMap.IsValidIndex(tableMapIndex)) {
        if (InitSchemaFieldsForClass(&schemaTableMap, className, classKey))
            return GetOffset(className, classKey, memberName, memberKey);

        return {0, 0};
    }

    SchemaKeyValueMap_t* tableMap = schemaTableMap[tableMapIndex];
    int16_t memberIndex = tableMap->Find(memberKey);
    if (!tableMap->IsValidIndex(memberIndex)) {
        LOG("schema::GetOffset(): '%s' was not found in '%s'!\n", memberName,
            className);
        return {0, 0};
    }

    return tableMap->Element(memberIndex);
}

bool CEconItemDefinition::IsWeapon() {
    // Every gun supports at least 4 stickers.
    return GetStickersSupportedCount() >= 4;
}

bool CEconItemDefinition::IsKnife(bool excludeDefault) {
    static constexpr auto CSGO_Type_Knife =
        hash_32_fnv1a_const("#CSGO_Type_Knife");

    if (hash_32_fnv1a_const(m_pszItemTypeName) != CSGO_Type_Knife) return false;
    return excludeDefault ? m_nDefIndex >= 500 : true;
}

bool CEconItemDefinition::IsGlove(bool excludeDefault) {
    static constexpr auto Type_Hands = hash_32_fnv1a_const("#Type_Hands");

    if (hash_32_fnv1a_const(m_pszItemTypeName) != Type_Hands) return false;
    const bool defaultGlove = m_nDefIndex == 5028 || m_nDefIndex == 5029;

    return excludeDefault ? !defaultGlove : true;
}

auto CLocalize::FindSafe(const char* tokenName) -> const char* {
    return CALL_VIRTUAL(const char*, 17, this, tokenName);
}
auto GetGameGlobals() -> CGlobalVars* {
    INetworkGameServer* server =
        Offset::InterFaces::INetworkServerServiceInteFace->GetIGameServer();

    if (!server) return nullptr;

    return Offset::InterFaces::INetworkServerServiceInteFace->GetIGameServer()
        ->GetGlobals();
}

auto SetStateChanged(Z_CBaseEntity* pEntity, int offset) -> void {
    Offset::FnStateChanged(pEntity->m_NetworkTransmitComponent(), pEntity,
                           offset, -1, -1);
    // auto vars = GetGameGlobals();
    auto vars = global::GlobalVars;

    if (vars) pEntity->m_lastNetworkChange(vars->curtime);

    pEntity->m_isSteadyState(0);
};
