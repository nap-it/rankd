#ifndef RANK_PRELUDE_TIMEOUT_HANDLER_SPEC_H
#define RANK_PRELUDE_TIMEOUT_HANDLER_SPEC_H

#include <thread>

class TimeoutHandlerSpec {
public:
    // Instance handling.
    static TimeoutHandlerSpec* get_instance();

    // Threading control mechanisms.
    TimeoutHandlerSpec* execute();
    TimeoutHandlerSpec* stop();
    bool is_running() const;
protected:
    void operator()();
private:
    TimeoutHandlerSpec();
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
};


#endif  // RANK_PRELUDE_TIMEOUT_HANDLER_SPEC_H
