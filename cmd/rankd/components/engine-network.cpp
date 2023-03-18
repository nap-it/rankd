#include "components/engine-network.h"

void NetworkEngine::operator()() {
    while (_is_running) {
        _logger->info("Network engine is operating.");
        // TODO

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_waiting_milliseconds));
    }
}

void NetworkEngine::execute() {
    _is_running = true;
    _thread = std::thread(std::ref(*this));
}

void NetworkEngine::stop() {
    if (!_is_running) {
        _logger->debug("Someone tried to stop the already stopped network engine thread.");
        return;
    }
    _logger->debug("Stopping the network engine thread.");
    _is_running = false;
    _thread.join();
}

const std::atomic<bool>& NetworkEngine::is_running() const {
    return _is_running;
}

NetworkEngine::~NetworkEngine() {
    if (_is_running) {
        NetworkEngine::stop();
    }
}
