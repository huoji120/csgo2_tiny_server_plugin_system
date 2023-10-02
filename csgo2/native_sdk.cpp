#include "native_sdk.h"

CBaseEntity* CHandle::GetBaseEntity() const
{
    CGameEntitySystem* pEntitySystem = CGameEntitySystem::GetInstance();
    if (!pEntitySystem)
        return nullptr;

    return pEntitySystem->GetBaseEntity(GetEntryIndex());
}
CGameEntitySystem* CGameEntitySystem::GetInstance() {
    return Offset::InterFaces::GameResourceServiceServer->GetGameEntitySystem();
}

auto CSchemaSystemTypeScope::FindDeclaredClass(const char* pClass) -> SchemaClassInfoData_t*
{
    SchemaClassInfoData_t* rv = nullptr;
    CALL_VIRTUAL(void, 2, this, &rv, pClass);
    return rv;
}

auto CSchemaSystem::FindTypeScopeForModule(const char* module) -> CSchemaSystemTypeScope*
{
    return CALL_VIRTUAL(CSchemaSystemTypeScope*, 13, this, module, nullptr);
}

bool CBaseEntity::IsBasePlayerController() {
    return CALL_VIRTUAL(bool, 144, this);
}

auto CBasePlayer::ForceRespawn() -> void
{
    return CALL_VIRTUAL(void, 26, this);
}

auto CCSPlayerPawn::GetPlayerController() -> CCSPlayerController*
{
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
                //printf("Found Pawn Player: %d %s \n", player->GetRefEHandle().GetEntryIndex(), &player->m_iszPlayerName());
                return player;
            }
        }
    }
    return nullptr;
}

using SchemaKeyValueMap_t = std::unordered_map<uint32_t, int16_t>;
using SchemaTableMap_t = std::unordered_map<uint32_t, SchemaKeyValueMap_t>;

static bool InitSchemaFieldsForClass(SchemaTableMap_t& tableMap,
    const char* className, uint32_t classKey) {
    CSchemaSystemTypeScope* pType =
        Offset::InterFaces::SchemaSystem->FindTypeScopeForModule("server.dll");
    if (!pType) return false;

    SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className);
    if (!pClassInfo) {
        tableMap.emplace(classKey, SchemaKeyValueMap_t{});

        LOG("InitSchemaFieldsForClass(): '%s' was not found!\n", className);
        return false;
    }

    short fieldsSize = pClassInfo->GetFieldsSize();
    SchemaClassFieldData_t* pFields = pClassInfo->GetFields();

    auto& keyValueMap = tableMap[classKey];
    keyValueMap.reserve(fieldsSize);

    for (int i = 0; i < fieldsSize; ++i) {
        SchemaClassFieldData_t& field = pFields[i];
        LOG("%s::%s found at -> 0x%X\n", className, field.m_name,
            field.m_offset);
        keyValueMap.emplace(hash_32_fnv1a_const(field.m_name), field.m_offset);
    }

    return true;
}

int16_t schema::FindChainOffset(const char* className)
{
    CSchemaSystemTypeScope* pType =
        Offset::InterFaces::SchemaSystem->FindTypeScopeForModule("server.dll");
    if (!pType) return false;

    SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className);

    do
    {
        SchemaClassFieldData_t* pFields = pClassInfo->GetFields();
        short fieldsSize = pClassInfo->GetFieldsSize();
        for (int i = 0; i < fieldsSize; ++i) {
            SchemaClassFieldData_t& field = pFields[i];

            if (strcmp(field.m_name, "__m_pChainEntity") == 0)
            {
                return field.m_offset;
            }

        }
    } while ((pClassInfo = pClassInfo->GetParent()) != nullptr);

    return 0;
}

int16_t schema::GetOffset(const char* className, uint32_t classKey,
    const char* memberName, uint32_t memberKey) {
    static SchemaTableMap_t schemaTableMap;
    const auto& tableMapIt = schemaTableMap.find(classKey);
    if (tableMapIt == schemaTableMap.cend()) {
        if (InitSchemaFieldsForClass(schemaTableMap, className, classKey))
            return GetOffset(className, classKey, memberName, memberKey);
        return 0;
    }

    const SchemaKeyValueMap_t& tableMap = tableMapIt->second;
    if (tableMap.find(memberKey) == tableMap.cend()) {
        LOG("schema::GetOffset(): '%s' was not found in '%s'!\n", memberName, className);

        return 0;
    }

    return tableMap.at(memberKey);
}