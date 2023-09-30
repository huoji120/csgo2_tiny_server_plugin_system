#include "handle.h"
#include "../cgameentitysystem.h"

CBaseEntity* CHandle::GetBaseEntity() const
{
	CGameEntitySystem* pEntitySystem = CGameEntitySystem::GetInstance();
	if (!pEntitySystem)
		return nullptr;

	return pEntitySystem->GetBaseEntity(GetEntryIndex());
}