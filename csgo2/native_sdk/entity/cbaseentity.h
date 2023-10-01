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
