#include "timer.h"
namespace ScriptApis {
extern auto TimerCallBack(_GameTimer* timer) -> void;
extern auto RunTickCallBack(_GameTickRunTime* timer) -> void;
};  // namespace ScriptApis
namespace GameTimer {
std::recursive_mutex mutex_timerList;
std::vector<_GameTimer*> timerList;
auto AddTimer(_GameTimer* timer) -> int {
    std::unique_lock<std::recursive_mutex> lock(mutex_timerList);
    timerList.push_back(timer);
    return timerList.size() - 1;
};
auto CleanUpTimers() -> void {
    std::unique_lock<std::recursive_mutex> lock(mutex_timerList);
    for (auto it = timerList.begin(); it != timerList.end();) {
        if ((*it)->m_bPreserveMapChange) {
            ++it;
        } else {
            delete (*it);
            it = timerList.erase(it);
        }
    }
};
auto ExcuteTimers() -> void {
    std::unique_lock<std::recursive_mutex> lock(mutex_timerList);
    for (auto it = timerList.begin(); it != timerList.end();) {
        if ((*it)->m_flLastExecute == -1) {
            (*it)->m_flLastExecute = global::m_flUniversalTime;
        }
        if ((*it)->m_flLastExecute + (*it)->m_flTime <=
            global::m_flUniversalTime) {
            ScriptApis::TimerCallBack(*it);
            if ((*it)->m_bRepeat) {
                (*it)->m_flLastExecute = global::m_flUniversalTime;
            } else {
                delete (*it);
                it = timerList.erase(it);
                continue;
            }
        }
        ++it;
    }
}
};  // namespace GameTimer
namespace GameTickRunTime {
std::recursive_mutex mutex_tickRunList;
std::vector<_GameTickRunTime*> tickRunList;
auto AddTickFunction(_GameTickRunTime* timer) -> void {
    std::unique_lock<std::recursive_mutex> lock(mutex_tickRunList);
    tickRunList.push_back(timer);
};
auto CleanUpTickFunctions() -> void {
    std::unique_lock<std::recursive_mutex> lock(mutex_tickRunList);
    for (auto it = tickRunList.begin(); it != tickRunList.end();) {
        delete (*it);
        it = tickRunList.erase(it);
    }
};
auto ExcuteTickFunctions() -> void {
    std::unique_lock<std::recursive_mutex> lock(mutex_tickRunList);
    for (auto it = tickRunList.begin(); it != tickRunList.end();) {
        ScriptApis::RunTickCallBack(*it);
        delete (*it);
        it = tickRunList.erase(it);
    }
}
};  // namespace GameTickRunTime
