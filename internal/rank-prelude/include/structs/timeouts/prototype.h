#ifndef RANK_PRELUDE_TIMEOUTS_PROTOTYPE_H
#define RANK_PRELUDE_TIMEOUTS_PROTOTYPE_H

#include <chrono>
#include <future>

#include "structs/timeouts/types.h"

template<class _Rep, class _Period>
std::future<void> timer(std::chrono::duration<_Rep, _Period> duration, std::function<void()> rollback_if_required) {
    return std::async(std::launch::async, [duration, rollback_if_required]() {
        // Apply the waiting time as a sleep to the current thread.
        std::this_thread::sleep_for(duration);

        // Execute the function that first checks if it is required, and then proceeds to rollback features.
        rollback_if_required();
    });
}


class Timeout {
public:
    Timeout* execute();
    Timeout* stop();
    Timeout* signal_stop();
    virtual void operator()() = 0;
    bool is_running();
protected:
    Timeout(const TimeoutType& type, const std::function<void()>& on_timeout);
    std::function<void()> _on_timeout;
private:
    TimeoutType _type;
    std::thread _thread;
    bool _running = false;
};

#endif //RANK_PRELUDE_TIMEOUTS_PROTOTYPE_H
