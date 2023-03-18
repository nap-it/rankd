#include "components/engine-computing.h"

void ComputingEngine::operator()() {
    while (_is_running) {
        _logger->info("Computing engine is operating.");
        // TODO

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_waiting_milliseconds));
    }
}

void ComputingEngine::execute() {
    _is_running = true;
    _thread = std::thread(std::ref(*this));
}

void ComputingEngine::stop() {
    if (!_is_running) {
        _logger->debug("Someone tried to stop the already stopped computing engine thread.");
        return;
    }
    _logger->debug("Stopping the computing engine thread.");
    _is_running = false;
    _thread.join();
}

const std::atomic<bool>& ComputingEngine::is_running() const {
    return _is_running;
}

ComputingEngine::~ComputingEngine() {
    if (_is_running) {
        ComputingEngine::stop();
    }
}
