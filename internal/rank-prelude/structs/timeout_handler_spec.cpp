#include "structs/timeout_handler_spec.h"

TimeoutHandlerSpec::TimeoutHandlerSpec() {

}

TimeoutHandlerSpec* TimeoutHandlerSpec::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

TimeoutHandlerSpec* TimeoutHandlerSpec::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool TimeoutHandlerSpec::is_running() const {
    return _running;
}

void TimeoutHandlerSpec::operator()() {}
