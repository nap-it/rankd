#ifndef RANK_PRELUDE_TIMEOUTS_MAR_H
#define RANK_PRELUDE_TIMEOUTS_MAR_H

#include "constants.h"
#include "structs/timeouts/prototype.h"

class MARTimeout : public Timeout {
public:
    static MARTimeout *initiate(const std::function<void()> &rollback_function);

    void operator()() override;

private:
    MARTimeout(const std::function<void()> &rollback_function) : Timeout(TimeoutType::MAR,
                                                                         rollback_function) {}
};

#endif //RANK_PRELUDE_TIMEOUTS_MAR_H
