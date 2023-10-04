#pragma once
#include "head.h"
class _GameTimer {
   public:
    float m_flTime;
    float m_flLastExecute = -1;
    bool m_bRepeat;
    bool m_bPreserveMapChange;
    lua_State* m_luaVm;
    int m_iParamIndex;
    int m_iLuaCallBackFn;
};
class _GameTickRunTime {
   public:
    lua_State* m_luaVm;
    int m_iParamIndex;
    int m_iLuaCallBackFn;
};
namespace GameTimer {
auto AddTimer(_GameTimer* timer) -> int;
auto CleanUpTimers() -> void;
auto ExcuteTimers() -> void;
};  // namespace GameTimer

namespace GameTickRunTime {
auto AddTickFunction(_GameTickRunTime* timer) -> void;
auto CleanUpTickFunctions() -> void;
auto ExcuteTickFunctions() -> void;
};  // namespace GameTickRunTime
