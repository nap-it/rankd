#ifndef RANK_PRELUDE_TIMEOUT_HANDLER_SPEC_H
#define RANK_PRELUDE_TIMEOUT_HANDLER_SPEC_H

#include <thread>

class TimeoutHandlerSpec {
public:
    // Instance handling.
    TimeoutHandlerSpec();

    // Threading control mechanisms.
    TimeoutHandlerSpec* execute();
    TimeoutHandlerSpec* stop();
    bool is_running() const;
    void operator()();
private:
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
};


#endif  // RANK_PRELUDE_TIMEOUT_HANDLER_SPEC_H
