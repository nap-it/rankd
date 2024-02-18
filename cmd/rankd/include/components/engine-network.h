#ifndef RANKD_ENGINE_NETWORK_H
#define RANKD_ENGINE_NETWORK_H

#include "engine.h"

#include "net-lib.h"

class NetworkEngine : public Engine {
public:
    static NetworkEngine* get_instance() {
        static auto* instance = new NetworkEngine();
        return instance;
    }
    void operator()() override;
    void execute() override;
    void stop() override;
    void teller(const std::shared_ptr<restbed::Session>& session) override;
    void tell_neighbors(const std::shared_ptr<restbed::Session>& session);
    void tell_routes(const std::shared_ptr<restbed::Session>& session);
    void tell_interfaces(const std::shared_ptr<restbed::Session>& session);
    std::vector<std::shared_ptr<restbed::Resource>>* get_resources() override;
    [[nodiscard]] const std::atomic<bool>& is_running() const override;
    ~NetworkEngine();
private:
    NetworkEngine() = default;
    NetworkNeighbors _neighbors = NetworkNeighbors();
    NetworkRoutes _routes = NetworkRoutes();
    NetworkDevices _interfaces = NetworkDevices();
};

#endif  // RANKD_ENGINE_NETWORK_H
