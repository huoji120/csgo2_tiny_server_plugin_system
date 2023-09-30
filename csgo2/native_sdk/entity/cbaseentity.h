#pragma once

#include "../../schema.h"
#include "ccollisionproperty.h"
#include "../handle/handle.h"
#include "../cgameentitysystem.h"
class CEntityIdentity;
class CEntityInstance {
public:
	DECLARE_CLASS(CEntityInstance);

	auto Schema_DynamicBinding() {
		SchemaClassInfoData_t* rv = nullptr;
		CALL_VIRTUAL(void, 0, this, &rv);
		return rv;
	}

	auto GetRefEHandle() {
		CHandle handle;
		CALL_VIRTUAL(void*, 2, this, &handle);
		return handle;
	}

	SCHEMA_FIELD(CEntityIdentity*, m_pEntity);
	SCHEMA_FIELD(const char*, m_designerName);

};

class CBaseEntity : CEntityInstance
{
public:
	DECLARE_CLASS(CBaseEntity);

	SCHEMA_FIELD(int, m_iHealth)
	//SCHEMA_FIELD(Vector, m_vecBaseVelocity)
	SCHEMA_FIELD(CCollisionProperty*, m_pCollision)
};