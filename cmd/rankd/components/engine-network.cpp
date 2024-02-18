#include "components/engine-network.h"

void NetworkEngine::operator()() {
    while (_is_running) {
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

void NetworkEngine::tell_neighbors(const std::shared_ptr<restbed::Session> &session) {
    const auto request = session->get_request();
    _logger->info("A request was received for network neighbors information.");

    _neighbors.snap();

    rapidjson::Document json_document = _neighbors.json();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_document.Accept(writer);


    session->close(restbed::OK, buffer.GetString(),
                   {{"Content-Length", std::to_string(buffer.GetLength())}, {"Connection", "close"}});
}

void NetworkEngine::tell_routes(const std::shared_ptr<restbed::Session> &session) {
    const auto request = session->get_request();
    _logger->info("A request was received for network routes information.");

    _routes.snap();

    rapidjson::Document json_document = _routes.json();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_document.Accept(writer);


    session->close(restbed::OK, buffer.GetString(),
                   {{"Content-Length", std::to_string(buffer.GetLength())}, {"Connection", "close"}});
}

void NetworkEngine::tell_interfaces(const std::shared_ptr<restbed::Session> &session) {
    const auto request = session->get_request();
    _logger->info("A request was received for network interfaces information.");

    _interfaces.snap();

    rapidjson::Document json_document = _interfaces.json();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_document.Accept(writer);


    session->close(restbed::OK, buffer.GetString(),
                   {{"Content-Length", std::to_string(buffer.GetLength())}, {"Connection", "close"}});
}

std::vector<std::shared_ptr<restbed::Resource>>* NetworkEngine::get_resources() {
    auto* resources = new std::vector<std::shared_ptr<restbed::Resource>>();

    _logger->trace("The network engine is configuring its API resource.");
    auto resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/net");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->teller(session); });
    _logger->trace("-- Setting /net");
    resources->push_back(resource);

    resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/net/neighbors");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->tell_neighbors(session); });
    _logger->trace("-- Setting /net/neighbors");
    resources->push_back(resource);

    resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/net/routes");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->tell_routes(session); });
    _logger->trace("-- Setting /net/routes");
    resources->push_back(resource);

    resource = std::make_shared<restbed::Resource>();
    resource->set_path("api/v1/net/interfaces");
    resource->set_method_handler(
            "GET", [this](const std::shared_ptr<restbed::Session>& session) { return this->tell_interfaces(session); });
    _logger->trace("-- Setting /net/interfaces");
    resources->push_back(resource);

    _logger->info("The network engine is now starting all its services.");

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
