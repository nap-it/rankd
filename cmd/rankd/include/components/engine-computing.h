#ifndef RANKD_ENGINE_COMPUTING_H
#define RANKD_ENGINE_COMPUTING_H

#include "engine.h"

class ComputingEngine : public Engine {
public:
    static ComputingEngine* get_instance() {
        static auto* instance = new ComputingEngine();
        return instance;
    }
    void operator()() override;
    void execute() override;
    void stop() override;
    [[nodiscard]] const std::atomic<bool>& is_running() const override;
    ~ComputingEngine();
private:
    ComputingEngine() = default;
};

#endif  // RANKD_ENGINE_COMPUTING_H
