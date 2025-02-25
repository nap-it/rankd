#ifndef RANK_PRELUDE_TIMEOUTS_AUCT_H
#define RANK_PRELUDE_TIMEOUTS_AUCT_H

#include "constants.h"
#include "structs/timeouts/prototype.h"

class AUCTTimeout : public Timeout {
public:
    static AUCTTimeout *
    initiate(const std::function<void()> &on_timeout_function, const std::function<bool()> &all_bids_arrived);

    void operator()() override;

private:
    AUCTTimeout(const std::function<void()> &on_timeout_function, const std::function<bool()> &all_bids_arrived)
            : Timeout(TimeoutType::AUCT,
                      on_timeout_function), _all_bids_arrived(all_bids_arrived) {}

    std::function<bool()> _all_bids_arrived;
};

#endif //RANK_PRELUDE_TIMEOUTS_AUCT_H
