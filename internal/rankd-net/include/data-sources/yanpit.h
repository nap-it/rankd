#ifndef RANKD_NET_LIB_YANPIT_H
#define RANKD_NET_LIB_YANPIT_H

#include <atomic>
#include <mutex>
#include <thread>

#include "yanpit/dds/dds.h"

class IperfRepresentation {
public:
    static IperfRepresentation* get_instance() {
        static IperfRepresentation instance;
        return &instance;
    }
    IperfRepresentation(const IperfRepresentation& ncm) = delete;
    void operator=(const IperfRepresentation& ncm) = delete;
    void operator()();
    IperfRepresentation* execute();
    IperfRepresentation* stop();
    const std::atomic<bool>& is_running() const {
        return _running;
    }

    // Methods to retrieve data from Iperf status.
    Iperf::IperfStatus snapshot_iperf_status() {
        std::lock_guard<std::mutex> guard(*_iperf_status_mutex);

        return *_iperf_status_data;
    }

    ~IperfRepresentation();
private:
    IperfRepresentation() = default;
    std::atomic<bool> _running = false;
    std::thread _thread;
    int _thread_sampling_rate = 500;

    // NCM status DDS subscription.
    std::mutex* _iperf_status_mutex = nullptr;
    Iperf::IperfStatus* _iperf_status_data = nullptr;
    IperfSubscriber _iperf_subcriber;
};

class NcmRepresentation {
public:
    static NcmRepresentation* get_instance() {
        static NcmRepresentation instance;
        return &instance;
    }
    NcmRepresentation(const NcmRepresentation& ncm) = delete;
    void operator=(const NcmRepresentation& ncm) = delete;
    void operator()();
    NcmRepresentation* execute();
    NcmRepresentation* stop();
    const std::atomic<bool>& is_running() const {
        return _running;
    }

    // Methods to retrieve data from Localnet status.
    Ncm::NcmStatus snapshot_ncm_status() {
        std::lock_guard<std::mutex> guard(*_ncm_status_mutex);

        return *_ncm_status_data;
    }

    ~NcmRepresentation();
private:
    NcmRepresentation() = default;
    std::atomic<bool> _running = false;
    std::thread _thread;
    int _thread_sampling_rate = 500;

    // NCM status DDS subscription.
    std::mutex* _ncm_status_mutex = nullptr;
    Ncm::NcmStatus* _ncm_status_data = nullptr;
    NcmSubscriber _ncm_subcriber;
};

class LocalnetRepresentation {
public:
    static LocalnetRepresentation* get_instance() {
        static LocalnetRepresentation instance;
        return &instance;
    }
    LocalnetRepresentation(const LocalnetRepresentation& localnet) = delete;
    void operator=(const LocalnetRepresentation& localnet) = delete;
    void operator()();
    LocalnetRepresentation* execute();
    LocalnetRepresentation* stop();
    const std::atomic<bool>& is_running() const {
        return _running;
    }

    // Methods to retrieve data from Localnet status.
    Localnet::LocalnetStatus snapshot_localnet_status() {
        std::lock_guard<std::mutex> guard(*_localnet_status_mutex);

        return *_localnet_status_data;
    }

    ~LocalnetRepresentation();
private:
    LocalnetRepresentation() = default;
    std::atomic<bool> _running = false;
    std::thread _thread;
    int _thread_sampling_rate = 500;

    // Localnet status DDS subscription.
    std::mutex* _localnet_status_mutex = nullptr;
    Localnet::LocalnetStatus* _localnet_status_data = nullptr;
    LocalnetSubscriber _localnet_subcriber;
};

class YANPitRepresentation {
public:
    static YANPitRepresentation* get_instance() {
        static YANPitRepresentation instance;
        return &instance;
    }
    YANPitRepresentation(const YANPitRepresentation& yanpit) = delete;
    void operator=(const YANPitRepresentation& yanpit) = delete;
    void operator()();
    YANPitRepresentation* execute();
    YANPitRepresentation* stop();
    [[nodiscard]] const std::atomic<bool>& is_running() const {
        return _running;
    }

    // Methods to retrieve data from YANP-it status.
    Yanpit::YanpitStatus snapshot_yanpit_status() {
        std::lock_guard<std::mutex> guard(*_yanpit_status_mutex);

        return *_yanpit_status_data;
    }

    ~YANPitRepresentation();
private:
    YANPitRepresentation() = default;
    std::atomic<bool> _running = false;
    std::thread _thread;
    int _thread_sampling_rate = 500;

    // YANP-it status DDS subscription.
    std::mutex* _yanpit_status_mutex = nullptr;
    Yanpit::YanpitStatus* _yanpit_status_data = nullptr;
    YanpitSubscriber _yanpit_subscriber;
};

#endif  // RANKD_NET_LIB_YANPIT_H
