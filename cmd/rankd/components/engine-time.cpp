#include "components/engine-time.h"

void TimeEngine::operator()() {
    while (_is_running) {
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

void TimeEngine::teller(const std::shared_ptr<restbed::Session>& session) {
    const auto request = session->get_request();
    _logger->info("A request was received for time information.");

    session->close(restbed::OK, "", {{"Content-Length", std::to_string(0)}, {"Connection", "close"}});
}

void TimeEngine::tell_ptp(const std::shared_ptr<restbed::Session>& session) {
    const auto request = session->get_request();
    _logger->info("A request was received for CPU information.");

    _ptp.snap();

    rapidjson::Document json_document = _ptp.json();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_document.Accept(writer);

    session->close(restbed::OK, buffer.GetString(),
                   {{"Content-Length", std::to_string(buffer.GetLength())}, {"Connection", "close"}});
}

std::vector<std::shared_ptr<restbed::Resource>>* TimeEngine::get_resources() {
    auto* resources = new std::vector<std::shared_ptr<restbed::Resource>>();

    _logger->trace("The time engine is configuring its API resource.");

    auto resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/time");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->teller(session); });
    _logger->trace("-- Setting /time");
    resources->push_back(resource);

    resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/time/ptp");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->tell_ptp(session); });
    _logger->trace("-- Setting /time/ptp");
    resources->push_back(resource);

    _logger->info("The time engine is now starting all its services.");

    return resources;
}

const std::atomic<bool>& TimeEngine::is_running() const {
    return _is_running;
}

TimeEngine::~TimeEngine() {
    if (_is_running) {
        TimeEngine::stop();
    }
}
