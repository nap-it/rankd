#ifndef RANKD_NET_LIB_H
#define RANKD_NET_LIB_H

#include <atomic>
#include <thread>

#include "data-sources/yanpit.h"

class YanpitConnection {
public:
    static YanpitConnection* get_instance() {
        static YanpitConnection instance;
        return &instance;
    }
    YanpitConnection(const YanpitConnection& connection) = delete;
    void operator=(const YanpitConnection& connection) = delete;

    void operator()() {
        while (_running) {
            // TODO Do something.
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    YanpitConnection* execute() {
        _running = true;

        _yanpit_instance->execute();
        _iperf_instance->execute();
        _ncm_instance->execute();
        _localnet_instance->execute();

        _thread = std::thread(std::ref(*this));

        return get_instance();
    }

    YanpitConnection* stop() {
        if (!_running) {
            return get_instance();
        }

        _yanpit_instance->stop();
        _iperf_instance->stop();
        _ncm_instance->stop();
        _localnet_instance->stop();

        _running = false;
        _thread.join();

        return get_instance();
    }

    [[nodiscard]] const std::atomic<bool>& is_running() const {
        return _running;
    }

    ~YanpitConnection() {
        stop();
    }
private:
    YanpitConnection() {
        _yanpit_instance = YANPitRepresentation::get_instance();
        _iperf_instance = IperfRepresentation::get_instance();
        _ncm_instance = NcmRepresentation::get_instance();
        _localnet_instance = LocalnetRepresentation::get_instance();
    }

    // Yanpit representation instances.
    YANPitRepresentation* _yanpit_instance = nullptr;
    IperfRepresentation* _iperf_instance = nullptr;
    NcmRepresentation* _ncm_instance = nullptr;
    LocalnetRepresentation* _localnet_instance = nullptr;

    // Threading control.
    std::atomic<bool> _running = false;
    std::thread _thread;
};

#endif  // RANKD_NET_LIB_H
