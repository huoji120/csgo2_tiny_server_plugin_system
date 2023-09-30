#include "schema.h"
#include "native_sdk/cgameentitysystem.h"
#include "native_sdk/cschemasystem.h"
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