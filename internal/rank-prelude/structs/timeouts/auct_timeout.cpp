#include "structs/timeouts/auct_timeout.h"

AUCTTimeout *AUCTTimeout::initiate(const std::function<void()> &on_timeout_function, const std::function<bool()> &all_bids_arrived) {
    return new AUCTTimeout(on_timeout_function, all_bids_arrived);
}

void AUCTTimeout::operator()() {
    auto future = timer(RANK_AUCT_TIMEOUT_VALUE, _on_timeout);

    while (is_running()) {
        // Check the number of arrived bids is equal to the expected.
        if (_all_bids_arrived()) {
            signal_stop();
            break;
        }

        // Get status of the future value. If reaches the end, we're finished.
        auto status = future.wait_for(1ms);
        if (status == std::future_status::ready) {
            signal_stop();
        }
    }
}