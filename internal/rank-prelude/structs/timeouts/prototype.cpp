#include "structs/timeouts/prototype.h"

Timeout::Timeout(const TimeoutType& type, const std::function<void()> &on_timeout) {
    _type = type;
    _on_timeout = on_timeout;
}

Timeout *Timeout::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

Timeout *Timeout::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

Timeout *Timeout::signal_stop() {
    if (not _running) {
        return this;
    }

    _running = false;

    return this;
}

bool Timeout::is_running() {
    return _running;
}