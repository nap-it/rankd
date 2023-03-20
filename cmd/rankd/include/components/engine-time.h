#ifndef RANKD_ENGINE_TIME_H
#define RANKD_ENGINE_TIME_H

#include "engine.h"

class TimeEngine : public Engine {
public:
    static TimeEngine* get_instance() {
        static auto* instance = new TimeEngine();
        return instance;
    }
    void operator()() override;
    void execute() override;
    void stop() override;
    void teller(const std::shared_ptr<restbed::Session>& session) const override;
    std::vector<std::shared_ptr<restbed::Resource>>* get_resources() override;
    [[nodiscard]] const std::atomic<bool>& is_running() const override;
    ~TimeEngine();
private:
    TimeEngine() = default;
};

#endif  // RANKD_ENGINE_TIME_H
