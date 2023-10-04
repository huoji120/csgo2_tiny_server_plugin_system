#pragma once
#include "head.h"
class CEntityInstance;
typedef void(__fastcall* StateChanged_t)(void* networkTransmitComponent,
                                         CEntityInstance* ent, uint64_t offset,
                                         int a4, int a5);
typedef void(__fastcall* NetworkStateChanged_t)(uintptr_t chainEntity,
                                                uintptr_t offset, uintptr_t a3);
namespace Offset {
extern StateChanged_t FnStateChanged;
extern NetworkStateChanged_t FnNetworkStateChanged;
}  // namespace Offset
struct SchemaKey {
    int16_t offset;
    bool networked;
};
class Z_CBaseEntity;
extern auto SetStateChanged(Z_CBaseEntity* pEntity, int offset) -> void;

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS 64
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST* MAX_ENTITY_LISTS
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define ENT_ENTRY_MASK 0x7FFF

#define DECLARE_SCHEMA_CLASS_BASE(className, isStruct)   \
    static constexpr const char* ThisClass = #className; \
    static constexpr bool OffsetIsStruct = isStruct;

#define DECLARE_CLASS(className) DECLARE_SCHEMA_CLASS_BASE(className, false)

// Use this for classes that can be wholly included within other classes (like
// CCollisionProperty within CBaseModelEntity)
#define DECLARE_SCHEMA_CLASS_INLINE(className) \
    DECLARE_SCHEMA_CLASS_BASE(className, true)

#define SCHEMA_FIELD_OFFSET(type, varName, extra_offset)                       \
    std::add_lvalue_reference_t<type> varName() {                              \
        static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClass); \
        static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);       \
                                                                               \
        static const auto m_key =                                              \
            schema::GetOffset(ThisClass, datatable_hash, #varName, prop_hash); \
                                                                               \
        return *reinterpret_cast<std::add_pointer_t<type>>(                    \
            (uintptr_t)(this) + m_key.offset + extra_offset);                  \
    }                                                                          \
    void varName(type val) {                                                   \
        static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClass); \
        static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);       \
                                                                               \
        static const auto m_key =                                              \
            schema::GetOffset(ThisClass, datatable_hash, #varName, prop_hash); \
                                                                               \
        static const auto m_chain = schema::FindChainOffset(ThisClass);        \
                                                                               \
        if (m_chain != 0 && m_key.networked) {                                 \
            LOG("Found chain offset %d for %s::%s\n", m_chain, ThisClass,      \
                #varName);                                                     \
            Offset::FnNetworkStateChanged((uintptr_t)(this) + m_chain,         \
                                          m_key.offset + extra_offset,         \
                                          0xFFFFFFFF);                         \
        } else if (m_key.networked) {                                          \
            /* WIP: Works fine for most props, but inlined classes in the      \
               middle of a class will need to have their this pointer          \
               corrected by the offset .*/                                     \
            LOG("Attempting to call SetStateChanged on on %s::%s\n",           \
                ThisClass, #varName);                                          \
            if (!OffsetIsStruct)                                               \
                SetStateChanged((Z_CBaseEntity*)this,                          \
                                m_key.offset + extra_offset);                  \
            else                                                               \
                CALL_VIRTUAL(void, 1, this, m_key.offset + extra_offset,       \
                             0xFFFFFFFF, 0xFFFF);                              \
        }                                                                      \
        *reinterpret_cast<std::add_pointer_t<type>>(                           \
            (uintptr_t)(this) + m_key.offset + extra_offset) = val;            \
    }

#define SCHEMA_FIELD(type, varName) SCHEMA_FIELD_OFFSET(type, varName, 0)

#define PSCHEMA_FIELD_OFFSET(type, varName, extra_offset)                      \
    auto varName() {                                                           \
        static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClass); \
        static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);       \
                                                                               \
        static const auto m_key =                                              \
            schema::GetOffset(ThisClass, datatable_hash, #varName, prop_hash); \
                                                                               \
        return reinterpret_cast<std::add_pointer_t<type>>(                     \
            (uintptr_t)(this) + m_key.offset + extra_offset);                  \
    }

#define PSCHEMA_FIELD(type, varName) PSCHEMA_FIELD_OFFSET(type, varName, 0)

namespace schema {
int16_t FindChainOffset(const char* className);
SchemaKey GetOffset(const char* className, uint32_t classKey,
                    const char* memberName, uint32_t memberKey);
}  // namespace schema

struct CSchemaNetworkValue {
    union {
        const char* m_sz_value;
        int m_n_value;
        float m_f_value;
        std::uintptr_t m_p_value;
    };
};

struct SchemaMetadataEntryData_t {
    const char* m_name;
    CSchemaNetworkValue* m_value;
};

struct SchemaClassFieldData_t {
    const char* m_name;
    char pad0[0x8];
    short m_offset;
    int32_t m_metadata_size;
    SchemaMetadataEntryData_t* m_metadata;
};

class SchemaClassInfoData_t;

struct SchemaBaseClassInfoData_t {
    unsigned int m_offset;
    SchemaClassInfoData_t* m_class;
};

class SchemaClassInfoData_t {
   public:
    auto GetName() { return m_name; }

    auto GetFieldsSize() { return m_align; }

    auto GetFields() { return m_fields; }

    auto GetParent() -> SchemaClassInfoData_t* {
        if (!m_schema_parent) return nullptr;

        return m_schema_parent->m_class;
    }

   private:
    char pad_0x0000[0x8];  // 0x0000

    const char* m_name;  // 0x0008
    char* m_module;      // 0x0010

    int m_size;            // 0x0018
    std::int16_t m_align;  // 0x001C

    std::int16_t m_static_size;    // 0x001E
    std::int16_t m_metadata_size;  // 0x0020
    std::int16_t m_i_unk1;         // 0x0022
    std::int16_t m_i_unk2;         // 0x0024
    std::int16_t m_i_unk3;         // 0x0026

    SchemaClassFieldData_t* m_fields;  // 0x0028

    char pad_0x0030[0x8];                        // 0x0030
    SchemaBaseClassInfoData_t* m_schema_parent;  // 0x0038

    char pad_0x0038[0x10];  // 0x0038
};

class CSchemaSystemTypeScope {
   public:
    auto FindDeclaredClass(const char* pClass) -> SchemaClassInfoData_t*;
};

class CSchemaSystem {
   public:
    auto FindTypeScopeForModule(const char* module) -> CSchemaSystemTypeScope*;
};

template <typename T>
class CUtlVector_NativeSdk {
   public:
    auto begin() const { return m_data; }
    auto end() const { return m_data + m_size; }

    bool Exists(T val) const {
        for (const auto& it : *this)
            if (it == val) return true;
        return false;
    }
    bool Empty() const { return m_size == 0; }

    int m_size;
    char pad0[0x4];  // no idea
    T* m_data;
    char pad1[0x8];  // no idea
};

class CBaseEntity;
class CEntityIdentity {
   public:
    CBaseEntity* entity;  // 0
    void* dunno;          // 8
    int64_t unk0;         // 12
    int64_t unk1;         // 16
    const char* internalName;
    const char* entityName;
    void* unk2;
    void* unk3;
    void* unk4;
    void* unk5;
    CEntityIdentity* prevValid;
    CEntityIdentity* nextValid;
    void* unkptr;
    void* unkptr2;
    void* unkptr3;
};

class CEntityIdentities {
   public:
    CEntityIdentity m_pIdentities[MAX_ENTITIES_IN_LIST];
};

class EntityIdentityList {
   public:
    CEntityIdentities* m_pIdentityList;
};

class CGameEntitySystem {
   public:
    virtual void n_0();
    void* unk;
    CEntityIdentities* m_pEntityList[MAX_ENTITY_LISTS];

    CBaseEntity* GetBaseEntity(int index) {
        if (index <= -1 || index >= MAX_TOTAL_ENTITIES) return nullptr;

        int listToUse = (index / MAX_ENTITIES_IN_LIST);
        if (!m_pEntityList[listToUse]) return nullptr;

        if (m_pEntityList[listToUse]
                ->m_pIdentities[index % MAX_ENTITIES_IN_LIST]
                .entity)
            return m_pEntityList[listToUse]
                ->m_pIdentities[index % MAX_ENTITIES_IN_LIST]
                .entity;
        else
            return nullptr;
    }

    static CGameEntitySystem* GetInstance();
};
struct VPhysicsCollisionAttribute_t {
    DECLARE_CLASS(VPhysicsCollisionAttribute_t)

    SCHEMA_FIELD(uint8_t, m_nCollisionGroup)
};

class CCollisionProperty {
   public:
    DECLARE_CLASS(CCollisionProperty)

    SCHEMA_FIELD(VPhysicsCollisionAttribute_t, m_collisionAttribute)
    // SCHEMA_FIELD(SolidType_t, m_nSolidType)
    SCHEMA_FIELD(uint8_t, m_usSolidFlags)
    SCHEMA_FIELD(uint8_t, m_CollisionGroup)
};

class CHandle {
    CBaseEntity* GetBaseEntity() const;

   public:
    bool operator==(CHandle rhs) const { return m_Index == rhs.m_Index; }

    bool IsValid() const { return m_Index != INVALID_EHANDLE_INDEX; }

    int GetEntryIndex() const { return m_Index & ENT_ENTRY_MASK; }

    template <typename T = CBaseEntity>
    T* Get() const {
        return reinterpret_cast<T*>(GetBaseEntity());
    }
    uint32_t m_Index;
};
class CEntityInstance {
   public:
    DECLARE_CLASS(CEntityInstance);

    auto Schema_DynamicBinding() {
        SchemaClassInfoData_t* rv = nullptr;
        CALL_VIRTUAL(void, 34, this, &rv);
        return rv;
    }

    auto GetRefEHandle() {
        char* pIdentity = (char*)m_pEntity();
        auto v3 = *(uint32_t*)(pIdentity + 16);
        auto v4 = ENT_ENTRY_MASK;
        auto v5 = ((v3 >> 15) - (*(uint32_t*)(pIdentity + 48) & 1)) << 15;
        if (v3 != -1) {
            v4 = *(uint32_t*)(pIdentity + 16) & ENT_ENTRY_MASK;
        }

        return CHandle(v4 | v5);
    }

    SCHEMA_FIELD(CEntityIdentity*, m_pEntity);
    SCHEMA_FIELD(const char*, m_designerName);
};

class CBaseEntity : public CEntityInstance {
   public:
    DECLARE_CLASS(CBaseEntity);

    SCHEMA_FIELD(int, m_iHealth)
    SCHEMA_FIELD(int, m_iTeamNum)
    // SCHEMA_FIELD(Vector, m_vecBaseVelocity)
    SCHEMA_FIELD(CCollisionProperty*, m_pCollision)
    auto IsBasePlayerController() -> bool;
    auto SpawnClientEntity() -> void;
};

class CBasePlayerController : public CBaseEntity {
   public:
    DECLARE_CLASS(CBasePlayerController);

    SCHEMA_FIELD(uint64_t, m_steamID)
    SCHEMA_FIELD(CHandle, m_hPawn)
    SCHEMA_FIELD(char, m_iszPlayerName)
    SCHEMA_FIELD(CUtlString, m_szNetworkIDString)
};

class CCSPlayerController : public CBasePlayerController {
   public:
    DECLARE_CLASS(CCSPlayerController)

    SCHEMA_FIELD(uint32_t, m_iPawnHealth)
    SCHEMA_FIELD(bool, m_bPawnIsAlive)
    SCHEMA_FIELD(const char*, m_szClanName)
};

class CEconItemDefinition {
   public:
    bool IsWeapon();
    bool IsKnife(bool excludeDefault);
    bool IsGlove(bool excludeDefault);

    auto GetModelName() {
        return *reinterpret_cast<const char**>((uintptr_t)(this) + 0xD8);
    }

    auto GetStickersSupportedCount() {
        return *reinterpret_cast<int*>((uintptr_t)(this) + 0x100);
    }

    auto GetSimpleWeaponName() {
        return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x210);
    }

    auto GetLoadoutSlot() {
        return *reinterpret_cast<int*>((uintptr_t)(this) + 0x2E8);
    }

    char pad0[0x8];  // vtable
    void* m_pKVItem;
    uint16_t m_nDefIndex;
    CUtlVector_NativeSdk<uint16_t> m_nAssociatedItemsDefIndexes;
    bool m_bEnabled;
    const char* m_szPrefab;
    uint8_t m_unMinItemLevel;
    uint8_t m_unMaxItemLevel;
    uint8_t m_nItemRarity;
    uint8_t m_nItemQuality;
    uint8_t m_nForcedItemQuality;
    uint8_t m_nDefaultDropItemQuality;
    uint8_t m_nDefaultDropQuantity;
    CUtlVector_NativeSdk<void*> m_vecStaticAttributes;
    uint8_t m_nPopularitySeed;
    void* m_pPortraitsKV;
    const char* m_pszItemBaseName;
    bool m_bProperName;
    const char* m_pszItemTypeName;
    uint32_t m_unItemTypeID;
    const char* m_pszItemDesc;
};

class CEconItemView {
   public:
    DECLARE_CLASS(CEconItemView);
    auto GetCustomPaintKitIndex() { return CALL_VIRTUAL(int, 2, this); }
    auto GetStaticData() {
        return CALL_VIRTUAL(CEconItemDefinition*, 13, this);
    }
};
class CAttributeContainer {
   public:
    DECLARE_CLASS(CAttributeContainer);

    PSCHEMA_FIELD(CEconItemView, m_Item);
};
class CEconEntity {
   public:
    DECLARE_CLASS(CEconEntity);

    PSCHEMA_FIELD(CAttributeContainer, m_AttributeManager);
};

class CBasePlayerWeapon : public CEconEntity {
   public:
    DECLARE_CLASS(CBasePlayerWeapon);

    SCHEMA_FIELD(int, m_iClip1);
    SCHEMA_FIELD(int, m_iClip2);
    SCHEMA_FIELD(int, m_pReserveAmmo);
};

class CPlayer_WeaponServices {
   public:
    DECLARE_CLASS(CPlayer_WeaponServices)

    SCHEMA_FIELD(CHandle, m_hActiveWeapon);
    SCHEMA_FIELD(uint16_t, m_iAmmo);
};

class CBasePlayer {
   public:
    auto ForceRespawn() -> void;
};

class CPlayer_MovementServices {
   public:
    DECLARE_CLASS(CPlayer_MovementServices);
};

class CCSPlayerController_InGameMoneyServices {
   public:
    DECLARE_CLASS(CCSPlayerController_InGameMoneyServices);

    SCHEMA_FIELD(int, m_iAccount)
};

class CBasePlayerPawn : public CBaseEntity {
   public:
    DECLARE_CLASS(CBasePlayerPawn);

    SCHEMA_FIELD(CPlayer_MovementServices*, m_pMovementServices)
    SCHEMA_FIELD(CPlayer_WeaponServices*, m_pWeaponServices)
    SCHEMA_FIELD(uint8_t**, m_pItemServices)
    // m_hController
    SCHEMA_FIELD(CBasePlayerController*, m_hController)
};
// Size: 0x1568
class CCSPlayerPawnBase : public CBasePlayerPawn {
   public:
    DECLARE_CLASS(CCSPlayerPawnBase);
    SCHEMA_FIELD(bool, m_bRespawning)
    SCHEMA_FIELD(int, m_ArmorValue)
};
class CCSPlayerPawn : public CCSPlayerPawnBase {
   public:
    DECLARE_CLASS(CCSPlayerPawn);
    SCHEMA_FIELD(const char*, m_szLastPlaceName)
    auto GetPlayerController() -> CCSPlayerController*;
    auto ForceRespawnPlayer() {
        return CALL_VIRTUAL(void, 324, this);
    }
};

class CGameEntitySystem;

class CGameResourceService {
   public:
    CGameEntitySystem* GetGameEntitySystem() {
        return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(this) + 0x58);
    }
};

class CLocalize {
   public:
    auto FindSafe(const char* tokenName) -> const char*;
};

class Z_CBaseEntity : public CBaseEntity {
   public:
    DECLARE_CLASS(CBaseEntity)

    SCHEMA_FIELD(CBitVec<64>, m_isSteadyState)
    SCHEMA_FIELD(float, m_lastNetworkChange)
    PSCHEMA_FIELD(void*, m_NetworkTransmitComponent)
    SCHEMA_FIELD(int, m_iHealth)
    SCHEMA_FIELD(int, m_iTeamNum)
    SCHEMA_FIELD(Vector, m_vecBaseVelocity)
    SCHEMA_FIELD(CCollisionProperty*, m_pCollision)
};
class CCSGameRules {
   public:
    DECLARE_CLASS(CCSGameRules)
    SCHEMA_FIELD(bool, m_bForceTeamChangeSilent)
    auto PlayerRespawn(CCSPlayerPawn* PlayerPawn) {
        return CALL_VIRTUAL(void, 110, this, PlayerPawn);
    }
};
