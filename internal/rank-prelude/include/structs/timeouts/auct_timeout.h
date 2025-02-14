#ifndef RANK_PRELUDE_TIMEOUTS_AUCT_H
#define RANK_PRELUDE_TIMEOUTS_AUCT_H

#include "constants.h"
#include "structs/timeouts/prototype.h"

class AUCTTimeout : public Timeout {
public:
    static AUCTTimeout *initiate(const std::function<void()> &on_timeout_function);

    void operator()() override;

private:
    AUCTTimeout(const std::function<void()> &on_timeout_function) : Timeout(TimeoutType::AUCT,
                                                                         on_timeout_function) {}
};

#endif //RANK_PRELUDE_TIMEOUTS_AUCT_H
