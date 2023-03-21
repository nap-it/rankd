#include "api/api.h"

std::shared_ptr<restbed::Service> configure_api(const std::shared_ptr<restbed::Settings>& settings,
                                                const ComputingEngine* computing_engine,
                                                const NetworkEngine* network_engine, const TimeEngine* time_engine) {
}

ApiController* ApiController::execute() {
    _is_running = true;
    _thread = std::thread(std::ref(*this));

    return get_instance();
}

ApiController* ApiController::stop() {
    if (_is_running) {
        _is_running = false;
        _thread.join();
    }

    return get_instance();
}

void ApiController::operator()() {
    _logger->trace("The API controller is now configuring each resource.");

    while (_is_running) {
        for (const auto& resource : _api_resources) {
            _api_service.publish(resource);
        }

        if (!_api_resources.empty()) {
            _logger->info("The API controller has loaded and started all of its services. A total of {:d} endpoints.", _api_resources.size());
            _api_service.start(_api_settings);
            _logger->info("The API controller has suspended its thread.");
        }
    }
}

ApiController* ApiController::apply_settings() {
    _api_settings = std::make_shared<restbed::Settings>();

    _api_settings->set_port(7265);
    _api_settings->set_default_header("Connection", "close");
    _api_settings->set_worker_limit(std::thread::hardware_concurrency());

    return get_instance();
}

ApiController* ApiController::append_resources(const std::vector<std::shared_ptr<restbed::Resource>>* resources) {
    for (const auto& resource : *resources) {
        _api_resources.push_back(resource);
    }

    return get_instance();
}

ApiController* ApiController::append_resource(const std::shared_ptr<restbed::Resource>& resource) {
    _api_resources.push_back(resource);

    return get_instance();
}

ApiController::~ApiController() {
    stop();
}