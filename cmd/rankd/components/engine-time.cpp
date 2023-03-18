#include "components/engine-time.h"

void TimeEngine::operator()() {
    while (_is_running) {
        _logger->info("Time engine is operating.");
        // TODO

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_waiting_milliseconds));
    }
}

void TimeEngine::execute() {
    _is_running = true;
    _thread = std::thread(std::ref(*this));
}

void TimeEngine::stop() {
    if (!_is_running) {
        _logger->debug("Someone tried to stop the already stopped time engine thread.");
        return;
    }
    _logger->debug("Stopping the time engine thread.");
    _is_running = false;
    _thread.join();
}

const std::atomic<bool>& TimeEngine::is_running() const {
    return _is_running;
}

TimeEngine::~TimeEngine() {
    if (_is_running) {
        TimeEngine::stop();
    }
}
