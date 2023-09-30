#pragma once
#include "pch.h"
#include "schema.h"
#include "native_sdk/entity/cbaseentity.h"
class CBasePlayer {
public:
	auto ForceRespawn() -> void;
};

class CPlayer_MovementServices
{
public:
	DECLARE_CLASS(CPlayer_MovementServices);
};

class CCSPlayerController_InGameMoneyServices
{
public:
	DECLARE_CLASS(CCSPlayerController_InGameMoneyServices);

	SCHEMA_FIELD(int, m_iAccount)
};
class CBasePlayerPawn : public CBaseEntity
{
public:
	DECLARE_CLASS(CBasePlayerPawn);

	SCHEMA_FIELD(CPlayer_MovementServices*, m_pMovementServices)
	SCHEMA_FIELD(uint8_t*, m_pWeaponServices)
	SCHEMA_FIELD(uint8_t**, m_pItemServices)
};
class CCSPlayerPawn : public CBasePlayerPawn
{
public:
	DECLARE_CLASS(CCSPlayerPawn);
	SCHEMA_FIELD(const char*, m_szLastPlaceName)

};