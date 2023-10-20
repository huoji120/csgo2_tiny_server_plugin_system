#pragma once
#include "head.h"
class CGlobalVars;
class CGameEntitySystem;
namespace global {
	extern bool Exit;
	extern int MaxPlayers;
	extern CGameEntitySystem* EntitySystem;
	extern bool HasTicked;
	extern CGlobalVars* GlobalVars;
	extern float m_flUniversalTime;
	extern float m_flLastTickedTime;
	extern bool IsMetaModInit;
}