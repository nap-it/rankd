#ifndef RANKD_API_H
#define RANKD_API_H

#include <vector>

#include "restbed"
#include "spdlog/spdlog.h"

#include "internals.h"

std::shared_ptr<restbed::Service> configure_api(const std::shared_ptr<restbed::Settings>& settings,
                                                const ComputingEngine* computing_engine,
                                                const NetworkEngine* network_engine, const TimeEngine* time_engine);

class ApiController {
public:
    static ApiController* get_instance() {
        static ApiController instance;
        return &instance;
    }
    ApiController* execute();
    ApiController* stop();
    void operator()();
    ApiController* apply_settings();
    ApiController* append_resources(const std::vector<std::shared_ptr<restbed::Resource>>* resources);
    ApiController* append_resource(const std::shared_ptr<restbed::Resource>& resource);
    [[nodiscard]] const std::atomic<bool>& is_running() const {
        return _is_running;
    }
    ApiController(const ApiController& controller) = delete;
    void operator=(const ApiController& controller) = delete;
    ~ApiController();
private:
    ApiController() = default;
    std::thread _thread;
    std::atomic<bool> _is_running = false;
    restbed::Service _api_service;
    std::shared_ptr<restbed::Settings> _api_settings;
    std::vector<std::shared_ptr<restbed::Resource>> _api_resources;
    std::shared_ptr<spdlog::logger> _logger = spdlog::get("rankd-logger");
};


#endif  // RANKD_API_H
