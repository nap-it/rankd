#ifndef RANK_PRELUDE_TIMEOUTS_EAR_H
#define RANK_PRELUDE_TIMEOUTS_EAR_H

#include "constants.h"
#include "structs/timeouts/prototype.h"

class EARTimeout : public Timeout {
public:
    static EARTimeout *initiate(const std::function<void()> &rollback_function);

    void operator()() override;

private:
    EARTimeout(const std::function<void()> &rollback_function) : Timeout(TimeoutType::EAR,
                                                                         rollback_function) {}
};

#endif //RANK_PRELUDE_TIMEOUTS_EAR_H
