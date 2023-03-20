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

void NetworkEngine::teller(const std::shared_ptr<restbed::Session>& session) {
    const auto request = session->get_request();
    _logger->info("A request was received for network information.");

    session->close(restbed::OK, "", {{"Content-Length", std::to_string(0)}, {"Connection", "close"}});
}

std::vector<std::shared_ptr<restbed::Resource>>* NetworkEngine::get_resources() {
    auto* resources = new std::vector<std::shared_ptr<restbed::Resource>>();

    _logger->trace("The network engine is configuring its API resource.");
    auto resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/net");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->teller(session); });
    _logger->trace("-- Setting /net");
    _logger->info("The network engine is now starting all its services.");
    resources->push_back(resource);

    return resources;
}

const std::atomic<bool>& NetworkEngine::is_running() const {
    return _is_running;
}

NetworkEngine::~NetworkEngine() {
    if (_is_running) {
        NetworkEngine::stop();
    }
}
