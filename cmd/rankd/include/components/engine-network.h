#ifndef RANKD_ENGINE_NETWORK_H
#define RANKD_ENGINE_NETWORK_H

#include "engine.h"

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
    std::vector<std::shared_ptr<restbed::Resource>>* get_resources() override;
    [[nodiscard]] const std::atomic<bool>& is_running() const override;
    ~NetworkEngine();
private:
    NetworkEngine() = default;
};

#endif  // RANKD_ENGINE_NETWORK_H
