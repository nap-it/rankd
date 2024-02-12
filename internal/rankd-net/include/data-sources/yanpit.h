#ifndef RANKD_NET_LIB_YANPIT_H
#define RANKD_NET_LIB_YANPIT_H

#include <atomic>
#include <mutex>
#include <thread>

#include "structs/yanpit-interfaces.h"
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
    NcmSubscriber _ncm_subscriber;
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
        if (_running) {
            return get_instance();
        }
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

    [[nodiscard]] std::map<std::string, NetworkInterface> interfaces_information_as_map() const {
        auto snapshot = _localnet_instance->snapshot_localnet_status();
        std::map<std::string, NetworkInterface> interfaces;

        for (const auto& interface : snapshot.interfaces()) {
            // Create a network interface.
            auto temporary_interface = NetworkInterface(interface.interface_name());

            // Add attributes to mapped interface.
            temporary_interface.address(interface.interface_item().mac_address());
            temporary_interface.ip_4_addresses(interface.interface_item().ip4_addresses());
            temporary_interface.ip_6_addresses(interface.interface_item().ip6_addresses());
            temporary_interface.carrier(interface.interface_item().has_carrier());
            temporary_interface.duplex_type(interface.interface_item().duplex_type());
            temporary_interface.interface_index(interface.interface_item().interface_index());
            temporary_interface.enslaved_interface_index(interface.interface_item().enslaved_interface_index());
            temporary_interface.mtu(interface.interface_item().mtu());
            temporary_interface.operation_state(interface.interface_item().operation_state());
            temporary_interface.speed(interface.interface_item().speed());
            temporary_interface.rx_bytes(interface.interface_item().rx_bytes());
            temporary_interface.tx_bytes(interface.interface_item().tx_bytes());
            temporary_interface.rx_packets(interface.interface_item().rx_packets());
            temporary_interface.tx_packets(interface.interface_item().tx_packets());
            temporary_interface.rx_errors(interface.interface_item().rx_errors());
            temporary_interface.tx_errors(interface.interface_item().tx_errors());
            temporary_interface.rx_dropped(interface.interface_item().rx_dropped());
            temporary_interface.tx_dropped(interface.interface_item().tx_dropped());

            // Add interface to map.
            interfaces.insert({interface.interface_name(), temporary_interface});
        }

        return interfaces;
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

#endif  // RANKD_NET_LIB_YANPIT_H
