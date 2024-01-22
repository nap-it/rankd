#include "structs/timeout_handler.h"

TimeoutHandler* TimeoutHandler::get_instance() {
    static auto* instance = new TimeoutHandler();
    return instance;
}

void TimeoutHandler::initiate_timeout(Handler* handler, uint8_t timeout) {
    // TODO
}

void TimeoutHandler::terminate_timeout(Handler* handler, uint8_t timeout) {
    // TODO
}

TimeoutHandler* TimeoutHandler::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

TimeoutHandler* TimeoutHandler::stop() {
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

}

TimeoutHandler::~TimeoutHandler() {

}

TimeoutHandler::TimeoutHandler() {}