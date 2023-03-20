#include "components/engine-computing.h"

void ComputingEngine::operator()() {
    while (_is_running) {
        _logger->info("Computing engine is operating.");
        // TODO

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_waiting_milliseconds));
    }
}

std::vector<std::shared_ptr<restbed::Resource>>* ComputingEngine::get_resources() {
    auto* resources = new std::vector<std::shared_ptr<restbed::Resource>>();

    _logger->trace("The computing engine is configuring its API resource.");
    auto resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/comp");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->teller(session); });
    _logger->trace("-- Setting /comp");
    _logger->info("The computing engine is now starting all its services.");
    resources->push_back(resource);

    return resources;
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

void ComputingEngine::teller(const std::shared_ptr<restbed::Session>& session) const {
    const auto request = session->get_request();
    _logger->info("A request was received for computing information.");

    session->close(restbed::OK, "", {{"Content-Length", std::to_string(0)}, {"Connection", "close"}});
}

const std::atomic<bool>& ComputingEngine::is_running() const {
    return _is_running;
}

ComputingEngine::~ComputingEngine() {
    if (_is_running) {
        ComputingEngine::stop();
    }
}
