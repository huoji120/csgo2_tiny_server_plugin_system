#include "timer.h"
namespace ScriptApis {
    extern auto TimerCallBack(_GameTimer* timer) -> void;
};
namespace GameTimer {
std::shared_mutex mutex_timerList;
std::vector<_GameTimer*> timerList;
auto AddTimer(_GameTimer* timer) -> int {
    std::unique_lock<std::shared_mutex> lock(mutex_timerList);
    timerList.push_back(timer);
    return timerList.size() - 1;
};
auto CleanUpTimers() -> void {
    std::unique_lock<std::shared_mutex> lock(mutex_timerList);
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
    std::shared_lock<std::shared_mutex> lock(mutex_timerList);
    for (auto it = timerList.begin(); it != timerList.end();) {
        if ((*it)->m_flLastExecute == -1) {
            (*it)->m_flLastExecute = global::m_flUniversalTime;
        }
        if ((*it)->m_flLastExecute + (*it)->m_flTime <= global::m_flUniversalTime) {
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
