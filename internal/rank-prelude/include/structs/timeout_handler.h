#ifndef RANK_PRELUDE_TIMEOUT_HANDLER_H
#define RANK_PRELUDE_TIMEOUT_HANDLER_H

#include <thread>

#include "structs/handler.h"
#include "structs/timeout_table.h"

class TimeoutHandler {
public:
    // Instance handling.
    static TimeoutHandler* get_instance();

    // Timeout controls.
    void initiate_timeout(Handler* handler);
    void terminate_timeout(Handler* handler);

    // Threading control mechanisms.
    TimeoutHandler* execute();
    TimeoutHandler* stop();
    bool is_running() const;
protected:
    void operator()();
private:
    TimeoutHandler();
    TimeoutTable _timeout_table;
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
};


#endif  // RANK_PRELUDE_TIMEOUT_HANDLER_H
