#ifndef RANK_PRELUDE_TIMEOUT_HANDLER_H
#define RANK_PRELUDE_TIMEOUT_HANDLER_H

#include <map>
#include <thread>
#include <vector>

#include "spdlog/spdlog.h"

#include "structs/handler.h"
#include "structs/timeouts/all.h"

class TimeoutHandler {
public:
    // Instance handling.
    static TimeoutHandler* get_instance(const std::string& logger_name);

    // Timeout controls.
    void initiate_timeout(Handler* handler, const TimeoutType& timeout);

    // Threading control mechanisms.
    TimeoutHandler* execute();
    TimeoutHandler* stop();
    bool is_running() const;
    void operator()();

    // Destructor.
    ~TimeoutHandler();

private:
    explicit TimeoutHandler(const std::string& logger_name);
    std::map<TimeoutType, std::vector<Timeout*>> _timeouts = {
            {TimeoutType::AUCT, {}},
            {TimeoutType::BID, {}},
            {TimeoutType::EAR, {}},
            {TimeoutType::MAR, {}},
            {TimeoutType::REP, {}}
    };
    int a = 2, b = 3;
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
    std::shared_ptr<spdlog::logger> _logger;
};


#endif  // RANK_PRELUDE_TIMEOUT_HANDLER_H
