#ifndef RANK_PRELUDE_TIMEOUT_HANDLER_H
#define RANK_PRELUDE_TIMEOUT_HANDLER_H

#include <thread>

#include "spdlog/spdlog.h"

#include "structs/handler.h"
#include "structs/timeout_table.h"

class TimeoutHandler {
public:
    // Instance handling.
    static TimeoutHandler* get_instance(const std::string& logger_name);

    // Timeout controls.
    void initiate_timeout(Handler* handler, uint8_t timeout);
    void terminate_timeout(Handler* handler, uint8_t timeout);

    // Threading control mechanisms.
    TimeoutHandler* execute();
    TimeoutHandler* stop();
    bool is_running() const;
    void operator()();

    // Destructor.
    ~TimeoutHandler();

private:
    explicit TimeoutHandler(const std::string& logger_name);
    TimeoutTable _timeout_table;
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
    std::shared_ptr<spdlog::logger> _logger;
};


#endif  // RANK_PRELUDE_TIMEOUT_HANDLER_H
