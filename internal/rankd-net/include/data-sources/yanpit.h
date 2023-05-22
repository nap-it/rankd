#ifndef RANKD_NET_LIB_YANPIT_H
#define RANKD_NET_LIB_YANPIT_H

#include <atomic>
#include <mutex>
#include <thread>

#include "restbed"

class YANPit {
public:
    static YANPit* get_instance() {
        static YANPit instance;
        return &instance;
    }
    YANPit(const YANPit& yanpit) = delete;
    void operator=(const YANPit& yanpit) = delete;
    void operator()();
    YANPit* execute();
    YANPit* stop();
    [[nodiscard]] const std::atomic<bool>& is_running() const {
        return _running;
    }
    ~YANPit();
private:
    YANPit() = default;
    std::atomic<bool> _running = false;
    std::thread _thread;
    int _thread_sampling_rate = 500;
    std::mutex* _mutex_pointer = nullptr;
    bool _yanpit_was_brokered_before_rankd = false;
};

#endif  // RANKD_NET_LIB_YANPIT_H
