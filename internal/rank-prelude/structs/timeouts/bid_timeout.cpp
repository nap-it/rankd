#include "structs/timeouts/bid_timeout.h"

BIDTimeout *BIDTimeout::initiate(const std::function<void()> &rollback_function) {
    return new BIDTimeout(rollback_function);
}

void BIDTimeout::operator()() {
    auto future = timer(RANK_BID_TIMEOUT_VALUE, _on_timeout);

    while (is_running()) {
        std::this_thread::sleep_for(1s);

        // Get status of the future value. If reaches the end, we're finished.
        auto status = future.wait_for(1ms);
        if (status == std::future_status::ready) {
            signal_stop();
        }
    }
}