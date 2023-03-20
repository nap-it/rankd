#ifndef RANKD_ENGINE_H
#define RANKD_ENGINE_H

#include <atomic>
#include <chrono>
#include <thread>

#include "restbed"
#include "spdlog/spdlog.h"

class Engine {
public:
    virtual void operator()() = 0;
    virtual void execute() = 0;
    virtual void stop() = 0;
    virtual const std::atomic<bool>& is_running() const = 0;
    virtual void teller(const std::shared_ptr<restbed::Session>& session) const = 0;
    virtual std::vector<std::shared_ptr<restbed::Resource>>* get_resources() = 0;
protected:
    Engine() = default;
    std::atomic<bool> _is_running = false;
    std::thread _thread;
    unsigned int _thread_waiting_milliseconds = 500;
    std::shared_ptr<spdlog::logger> _logger = spdlog::get("rankd-logger");
};

#endif  // RANKD_ENGINE_H
