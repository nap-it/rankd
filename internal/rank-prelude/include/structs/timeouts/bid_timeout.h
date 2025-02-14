#ifndef RANK_PRELUDE_TIMEOUTS_BID_H
#define RANK_PRELUDE_TIMEOUTS_BID_H

#include "constants.h"
#include "structs/timeouts/prototype.h"

class BIDTimeout : public Timeout {
public:
    static BIDTimeout *initiate(const std::function<void()> &rollback_function);

    void operator()() override;

private:
    BIDTimeout(const std::function<void()> &rollback_function) : Timeout(TimeoutType::BID,
                                                                         rollback_function) {}
};

#endif //RANK_PRELUDE_TIMEOUTS_BID_H
