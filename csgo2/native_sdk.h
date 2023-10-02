#pragma once
#include "head.h"
namespace Offset {
	extern uint64_t NetworkStateChangedPtr;
}
#define DECLARE_CLASS(className) static constexpr auto ThisClass = #className;

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS	 64
#define MAX_TOTAL_ENTITIES	 MAX_ENTITIES_IN_LIST *MAX_ENTITY_LISTS
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define ENT_ENTRY_MASK		  0x7FFF
#define SCHEMA_FIELD_OFFSET(type, varName, extra_offset)														\
	std::add_lvalue_reference_t<type> varName()																	\
	{																											\
		static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClass);									\
		static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);										\
																												\
		static const auto m_offset =																			\
			schema::GetOffset(ThisClass, datatable_hash, #varName, prop_hash);									\
																												\
		return *reinterpret_cast<std::add_pointer_t<type>>(														\
			(uintptr_t)(this) + m_offset + extra_offset);														\
	}																											\
	void varName(type val)																						\
	{																											\
		static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClass);									\
		static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);										\
																												\
		static const auto m_offset =																			\
			schema::GetOffset(ThisClass, datatable_hash, #varName, prop_hash);									\
																												\
		static const auto m_chain =																				\
			schema::FindChainOffset(ThisClass);																	\
																												\
		if (m_chain != 0)																						\
		{																										\
			reinterpret_cast<FnNetworkStateChanged>(Offset::NetworkStateChangedPtr)((uintptr_t)(this) + m_chain, m_offset + extra_offset, 0xFFFFFFFF);	\
		}																										\
		*reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)(this) + m_offset + extra_offset) = val;			\
	}

#define SCHEMA_FIELD(type, varName) \
	SCHEMA_FIELD_OFFSET(type, varName, 0)

#define PSCHEMA_FIELD_OFFSET(type, varName, extra_offset) \
	auto varName()																\
	{																			\
		static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClass);	\
		static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);		\
																				\
		static const auto m_offset =											\
			schema::GetOffset(ThisClass, datatable_hash, #varName, prop_hash);	\
																				\
		return reinterpret_cast<std::add_pointer_t<type>>(						\
			(uintptr_t)(this) + m_offset + extra_offset);						\
	}

#define PSCHEMA_FIELD(type, varName) \
	PSCHEMA_FIELD_OFFSET(type, varName, 0)

typedef void(__fastcall* FnNetworkStateChanged)(uintptr_t chainEntity, uintptr_t offset, uintptr_t a3);
namespace schema
{
	int16_t FindChainOffset(const char* className);
	int16_t GetOffset(const char* className, uint32_t classKey, const char* memberName, uint32_t memberKey);
}


struct SchemaClassFieldData_t
{
	const char* m_name;
	char pad0[0x8];
	short m_offset;
	char pad1[0xE];
};

class SchemaClassInfoData_t;

struct SchemaBaseClassInfoData_t
{
	unsigned int m_offset;
	SchemaClassInfoData_t* m_class;
};

class SchemaClassInfoData_t
{
public:
	auto GetName()
	{
		return m_name;
	}

	auto GetFieldsSize()
	{
		return m_align;
	}

	auto GetFields()
	{
		return m_fields;
	}

	auto GetParent()
	{
		return m_schema_parent->m_class;
	}

private:
	char pad_0x0000[0x8];						// 0x0000

	const char* m_name;							// 0x0008
	char* m_module;								// 0x0010

	int m_size;									// 0x0018
	std::int16_t m_align;						// 0x001C

	std::int16_t m_static_size;					// 0x001E
	std::int16_t m_metadata_size;				// 0x0020
	std::int16_t m_i_unk1;						// 0x0022
	std::int16_t m_i_unk2;						// 0x0024
	std::int16_t m_i_unk3;						// 0x0026

	SchemaClassFieldData_t* m_fields;			// 0x0028

	char pad_0x0030[0x8];						// 0x0030
	SchemaBaseClassInfoData_t* m_schema_parent; // 0x0038

	char pad_0x0038[0x10];						// 0x0038
};

class CSchemaSystemTypeScope
{
public:
	auto FindDeclaredClass(const char* pClass)->SchemaClassInfoData_t*;
};

class CSchemaSystem
{
public:
	auto FindTypeScopeForModule(const char* module)->CSchemaSystemTypeScope*;
};

class CBaseEntity;
class CEntityIdentity
{
public:
	CBaseEntity* entity; //0
	void* dunno; //8
	int64_t unk0; //12
	int64_t unk1; //16
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

class CEntityIdentities
{
public:
	CEntityIdentity m_pIdentities[MAX_ENTITIES_IN_LIST];
};

class EntityIdentityList
{
public:
	CEntityIdentities* m_pIdentityList;
};

class CGameEntitySystem
{
public:
	virtual void n_0();
	void* unk;
	CEntityIdentities* m_pEntityList[MAX_ENTITY_LISTS];

	CBaseEntity* GetBaseEntity(int index)
	{
		if (index <= -1 || index >= MAX_TOTAL_ENTITIES)
			return nullptr;

		int listToUse = (index / MAX_ENTITIES_IN_LIST);
		if (!m_pEntityList[listToUse])
			return nullptr;

		if (m_pEntityList[listToUse]->m_pIdentities[index % MAX_ENTITIES_IN_LIST].entity)
			return m_pEntityList[listToUse]->m_pIdentities[index % MAX_ENTITIES_IN_LIST].entity;
		else
			return nullptr;
	}

	static CGameEntitySystem* GetInstance();
};
struct VPhysicsCollisionAttribute_t
{
	DECLARE_CLASS(VPhysicsCollisionAttribute_t)

	SCHEMA_FIELD(uint8_t, m_nCollisionGroup)
};

class CCollisionProperty
{
public:
	DECLARE_CLASS(CCollisionProperty)

	SCHEMA_FIELD(VPhysicsCollisionAttribute_t, m_collisionAttribute)
		//SCHEMA_FIELD(SolidType_t, m_nSolidType)
		SCHEMA_FIELD(uint8_t, m_usSolidFlags)
		SCHEMA_FIELD(uint8_t, m_CollisionGroup)
};

class CHandle
{
	CBaseEntity* GetBaseEntity() const;

public:
	bool operator==(CHandle rhs) const { return m_Index == rhs.m_Index; }

	bool IsValid() const { return m_Index != INVALID_EHANDLE_INDEX; }

	int GetEntryIndex() const { return m_Index & ENT_ENTRY_MASK; }

	template <typename T = CBaseEntity>
	T* Get() const
	{
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
    // SCHEMA_FIELD(Vector, m_vecBaseVelocity)
    SCHEMA_FIELD(CCollisionProperty*, m_pCollision)
    bool IsBasePlayerController();
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
	SCHEMA_FIELD(uint8_t*, m_pWeaponServices)
	SCHEMA_FIELD(uint8_t**, m_pItemServices)
};
class CCSPlayerPawn : public CBasePlayerPawn {
public:
	DECLARE_CLASS(CCSPlayerPawn);
	SCHEMA_FIELD(const char*, m_szLastPlaceName)
	auto GetPlayerController() -> CCSPlayerController*;
};

class CGameEntitySystem;

class CGameResourceService
{
public:
	CGameEntitySystem* GetGameEntitySystem()
	{
		return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(this) + 0x58);
	}
};