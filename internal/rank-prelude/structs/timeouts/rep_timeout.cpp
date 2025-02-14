#include "structs/timeouts/rep_timeout.h"

REPTimeout *REPTimeout::initiate(const std::function<void()> &on_timeout_function) {
    return new REPTimeout(on_timeout_function);
}

void REPTimeout::operator()() {
    auto future = timer(RANK_REP_TIMEOUT_VALUE, _on_timeout);

    while (is_running()) {
        std::this_thread::sleep_for(1s);

        // Get status of the future value. If reaches the end, we're finished.
        auto status = future.wait_for(1ms);
        if (status == std::future_status::ready) {
            signal_stop();
        }
    }
}