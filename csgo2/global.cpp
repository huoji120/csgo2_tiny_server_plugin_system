#include "global.h"
namespace global {
	bool Exit;
	int MaxPlayers;
	CGameEntitySystem* EntitySystem;
	bool HasTicked;
	CGlobalVars* GlobalVars;
	float m_flUniversalTime;
	float m_flLastTickedTime;
}