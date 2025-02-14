#ifndef RANK_PRELUDE_TIMEOUTS_REP_H
#define RANK_PRELUDE_TIMEOUTS_REP_H

#include "constants.h"
#include "structs/timeouts/prototype.h"

class REPTimeout : public Timeout {
public:
    static REPTimeout *initiate(const std::function<void()> &rollback_function);

    void operator()() override;

private:
    REPTimeout(const std::function<void()> &rollback_function) : Timeout(TimeoutType::REP,
                                                                         rollback_function) {}
};

#endif //RANK_PRELUDE_TIMEOUTS_REP_H
