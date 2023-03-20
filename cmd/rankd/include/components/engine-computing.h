#ifndef RANKD_ENGINE_COMPUTING_H
#define RANKD_ENGINE_COMPUTING_H

#include "engine.h"

#include "comp-lib.h"

class ComputingEngine : public Engine {
public:
    static ComputingEngine* get_instance() {
        static auto* instance = new ComputingEngine();
        return instance;
    }
    void operator()() override;
    void execute() override;
    void stop() override;
    // API calls.
    void teller(const std::shared_ptr<restbed::Session>& session) override;
    void tell_cpu(const std::shared_ptr<restbed::Session>& session);
    std::vector<std::shared_ptr<restbed::Resource>>* get_resources() override;
    [[nodiscard]] const std::atomic<bool>& is_running() const override;
    ~ComputingEngine();
private:
    CPU _cpu = CPU();
    Memory _memory = Memory();
    Storage _storage = Storage();
    Sensors _sensors = Sensors();
    OperativeSystem _os = OperativeSystem();
    ComputingEngine() = default;
};

#endif  // RANKD_ENGINE_COMPUTING_H
