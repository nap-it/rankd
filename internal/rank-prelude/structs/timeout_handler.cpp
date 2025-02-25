#include "structs/timeout_handler.h"

TimeoutHandler *TimeoutHandler::get_instance(const std::string &logger_name) {
#ifdef FROM_SIMUZILLA
    return new TimeoutHandler(logger_name);
#else
    static auto* instance = new TimeoutHandler(logger_name);
    return instance;
#endif
}

void TimeoutHandler::initiate_timeout(Handler *handler, const TimeoutType &timeout) {
    // Get the type of the requested timeout.
    Timeout *event;
    switch (timeout) {
        case TimeoutType::EAR:
            event = static_cast<Timeout *>(EARTimeout::initiate([handler]() {
                handler->on_ear_timeout();
                // TODO Drop here the code to check if ACC or REF message was received for the UUID and, if not, "Delete UUID from the store".
            }))->execute();
            _timeouts.at(timeout).push_back(event);
            break;
        case TimeoutType::MAR:
            event = static_cast<Timeout *>(MARTimeout::initiate([handler]() {
                handler->on_mar_timeout();
                // TODO Drop here the code to check if ACC or REF message was received for the UUID and, if not, "Delete UUID from the store".
            }))->execute();
            _timeouts.at(timeout).push_back(event);
            break;
        case TimeoutType::BID:
            event = static_cast<Timeout *>(BIDTimeout::initiate([handler]() {
                handler->on_bid_timeout();
                // TODO Drop here the code to check if EAR message was received for the UUID and, if not, "Delete UUID from the store".
            }))->execute();
            _timeouts.at(timeout).push_back(event);
            break;
        case TimeoutType::AUCT:
            event = static_cast<Timeout *>(AUCTTimeout::initiate([handler]() {
                                                                     handler->on_auct_timeout();
                                                                 },
                                                                 [handler]() {
                                                                     return handler->all_bids_arrived();
                                                                 }))->execute();
            _timeouts.at(timeout).push_back(event);
            break;
        case TimeoutType::REP:
            event = static_cast<Timeout *>(REPTimeout::initiate([handler]() {
                handler->on_rep_timeout();
            }))->execute();
            _timeouts.at(timeout).push_back(event);
            break;
    }
}

TimeoutHandler *TimeoutHandler::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

TimeoutHandler *TimeoutHandler::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool TimeoutHandler::is_running() const {
    return _running;
}

void TimeoutHandler::operator()() {
    while (_running) {
        std::this_thread::sleep_for(1s);
    }
}

TimeoutHandler::~TimeoutHandler() {

}

TimeoutHandler::TimeoutHandler(const std::string &logger_name) {
    // Configure logging.
    _logger = spdlog::get(logger_name);
}