#include "components/engine-network.h"

void NetworkEngine::operator()() {
    while (_is_running) {
        _logger->info("Network engine is operating.");
        // TODO

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_waiting_milliseconds));
    }
}

std::shared_ptr<restbed::Resource> NetworkEngine::execute() {
    _is_running = true;
    _thread = std::thread(std::ref(*this));

    _logger->trace("The network engine is configuring its API resource.");
    auto resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/net");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->teller(session); });
    _logger->trace("-- Setting /net");
    _logger->info("The network engine is now starting all its services.");

    return resource;
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

void NetworkEngine::teller(const std::shared_ptr<restbed::Session>& session) const {
    const auto request = session->get_request();
    _logger->info("A request was received for network information.");

    session->close(restbed::OK, "", {{"Content-Length", std::to_string(0)}, {"Connection", "close"}});
}

const std::atomic<bool>& NetworkEngine::is_running() const {
    return _is_running;
}

NetworkEngine::~NetworkEngine() {
    if (_is_running) {
        NetworkEngine::stop();
    }
}
