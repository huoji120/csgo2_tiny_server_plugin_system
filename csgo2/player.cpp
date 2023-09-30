#include "player.h"

auto CBasePlayer::ForceRespawn() -> void
{
	return CALL_VIRTUAL(void, 26, this);
}