#include "cbaseentity.h"
bool CBaseEntity::IsBasePlayerController() {
    return CALL_VIRTUAL(bool, 144, this);
}
