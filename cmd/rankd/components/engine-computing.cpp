#include "components/engine-computing.h"

#include <iostream>

void ComputingEngine::operator()() {
    while (_is_running) {
        // TODO

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_waiting_milliseconds));
    }
}

std::vector<std::shared_ptr<restbed::Resource>>* ComputingEngine::get_resources() {
    auto* resources = new std::vector<std::shared_ptr<restbed::Resource>>();

    _logger->trace("The computing engine is configuring its main API resource.");
    auto resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/comp");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->teller(session); });
    _logger->trace("-- Setting /comp");
    resources->push_back(resource);

    resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/comp/cpu");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->tell_cpu(session); });
    _logger->trace("-- Setting /comp/cpu");
    resources->push_back(resource);

    resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/comp/memory");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->tell_memory(session); });
    _logger->trace("-- Setting /comp/memory");
    resources->push_back(resource);

    resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/comp/os");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->tell_os(session); });
    _logger->trace("-- Setting /comp/os");
    resources->push_back(resource);

    resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/comp/sensors");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->tell_sensors(session); });
    _logger->trace("-- Setting /comp/sensors");
    resources->push_back(resource);

    resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/comp/storage");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->tell_storage(session); });
    _logger->trace("-- Setting /comp/storage");
    resources->push_back(resource);

    _logger->info("The computing engine is now starting all its services.");

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

void ComputingEngine::teller(const std::shared_ptr<restbed::Session>& session) {
    const auto request = session->get_request();
    _logger->info("A request was received for computing information.");

    session->close(restbed::OK, "", {{"Content-Length", std::to_string(0)}, {"Connection", "close"}});
}

void ComputingEngine::tell_cpu(const std::shared_ptr<restbed::Session>& session) {
    const auto request = session->get_request();
    _logger->info("A request was received for CPU information.");

    _cpu.snap();

    rapidjson::Document json_document = _cpu.json();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_document.Accept(writer);


    session->close(restbed::OK, buffer.GetString(),
                   {{"Content-Length", std::to_string(buffer.GetLength())}, {"Connection", "close"}});
}

void ComputingEngine::tell_memory(const std::shared_ptr<restbed::Session>& session) {
    const auto request = session->get_request();
    _logger->info("A request was received for Memory information.");

    _memory.snap();

    auto json_document = _memory.json();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_document.Accept(writer);

    session->close(restbed::OK, buffer.GetString(),
                   {{"Content-Length", std::to_string(buffer.GetLength())}, {"Connection", "close"}});
}

void ComputingEngine::tell_os(const std::shared_ptr<restbed::Session>& session) {
    const auto request = session->get_request();
    _logger->info("A request was received for OS information.");

    _os.snap();

    auto json_document = _os.json();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_document.Accept(writer);

    session->close(restbed::OK, buffer.GetString(),
                   {{"Content-Length", std::to_string(buffer.GetLength())}, {"Connection", "close"}});
}

void ComputingEngine::tell_sensors(const std::shared_ptr<restbed::Session>& session) {
    const auto request = session->get_request();
    _logger->info("A request was received for Sensors information.");

    _sensors.snap();

    auto json_document = _sensors.json();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_document.Accept(writer);

    session->close(restbed::OK, buffer.GetString(),
                   {{"Content-Length", std::to_string(buffer.GetLength())}, {"Connection", "close"}});
}

void ComputingEngine::tell_storage(const std::shared_ptr<restbed::Session>& session) {
    const auto request = session->get_request();
    _logger->info("A request was received for Storage information.");

    _storage.snap();

    auto json_document = _storage.json();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_document.Accept(writer);

    session->close(restbed::OK, buffer.GetString(),
                   {{"Content-Length", std::to_string(buffer.GetLength())}, {"Connection", "close"}});
}

const std::atomic<bool>& ComputingEngine::is_running() const {
    return _is_running;
}

ComputingEngine::~ComputingEngine() {
    if (_is_running) {
        ComputingEngine::stop();
    }
}
