#include "structs/timeouts/auct_timeout.h"

AUCTTimeout *AUCTTimeout::initiate(const std::function<void()> &on_timeout_function) {
    return new AUCTTimeout(on_timeout_function);
}

void AUCTTimeout::operator()() {
    auto future = timer(RANK_AUCT_TIMEOUT_VALUE, _on_timeout);

    while (is_running()) {
        // Get status of the future value. If reaches the end, we're finished.
        auto status = future.wait_for(1ms);
        if (status == std::future_status::ready) {
            signal_stop();
        }
    }
}