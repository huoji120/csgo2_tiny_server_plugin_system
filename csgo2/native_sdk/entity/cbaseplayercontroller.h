#pragma once
#include "../../pch.h"
#include "../handle/handle.h"
#include "cbaseentity.h"
class CBasePlayerController : public CBaseEntity
{
public:
	DECLARE_CLASS(CBasePlayerController);

	SCHEMA_FIELD(uint64_t, m_steamID)
	SCHEMA_FIELD(CHandle, m_hPawn)
	SCHEMA_FIELD(char, m_iszPlayerName)

};

class CCSPlayerController : public CBasePlayerController {
public:
    DECLARE_CLASS(CCSPlayerController)

	SCHEMA_FIELD(uint32_t, m_iPawnHealth)
	SCHEMA_FIELD(bool, m_bPawnIsAlive)
	SCHEMA_FIELD(const char*, m_szClanName)

};
