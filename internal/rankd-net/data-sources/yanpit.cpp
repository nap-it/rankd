#include "data-sources/yanpit.h"

IperfRepresentation *IperfRepresentation::execute() {
    _running = true;
    _thread = std::thread(std::ref(*this));

    // Start Iperf DDS connection.
    _iperf_subcriber.execute();
    _iperf_subcriber.share(_iperf_status_data, _iperf_status_mutex);

    return get_instance();
}

IperfRepresentation *IperfRepresentation::stop() {
    // 1. If it is already stopped, quit.
    if (!_running) {
        return get_instance();
    }

    // 2. Stop dependencies and then this instance.
    _iperf_subcriber.stop();
    _running = false;
    _thread.join();

    return get_instance();
}

void IperfRepresentation::operator()() {
    while (_running) {
        // TODO Do something...

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_sampling_rate));
    }
}

IperfRepresentation::~IperfRepresentation() {
    stop();
}

NcmRepresentation *NcmRepresentation::execute() {
    _running = true;
    _thread = std::thread(std::ref(*this));

    // Start NCM DDS connection.
    _ncm_subscriber.execute();
    _ncm_subscriber.share(_ncm_status_data, _ncm_status_mutex);

    return get_instance();
}

NcmRepresentation *NcmRepresentation::stop() {
    // 1. If it is already stopped, quit.
    if (!_running) {
        return get_instance();
    }

    // 2. Stop dependencies and then this instance.
    _ncm_subscriber.stop();
    _running = false;
    _thread.join();

    return get_instance();
}

void NcmRepresentation::operator()() {
    while (_running) {
        // TODO Do something...

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_sampling_rate));
    }
}

NcmRepresentation::~NcmRepresentation() {
    stop();
}

LocalnetRepresentation* LocalnetRepresentation::execute() {
    _running = true;
    _thread = std::thread(std::ref(*this));

    // Start Localnet DDS connection.
    _localnet_subcriber.execute();
    _localnet_subcriber.share(_localnet_status_data, _localnet_status_mutex);

    return get_instance();
}

LocalnetRepresentation *LocalnetRepresentation::stop() {
    // 1. If it is already stopped, quit.
    if (!_running) {
        return get_instance();
    }

    // 2. Stop dependencies and then this instance.
    _localnet_subcriber.stop();
    _running = false;
    _thread.join();

    return get_instance();
}

void LocalnetRepresentation::operator()() {
    while (_running) {
        // TODO Do something...

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_sampling_rate));
    }
}

LocalnetRepresentation::~LocalnetRepresentation() {
    stop();
}

YANPitRepresentation* YANPitRepresentation::execute() {
    _running = true;
    _thread = std::thread(std::ref(*this));

    // Start YANP-it DDS connection.
    _yanpit_subscriber.execute();
    _yanpit_subscriber.share(_yanpit_status_data, _yanpit_status_mutex);

    return get_instance();
}

YANPitRepresentation* YANPitRepresentation::stop() {
    // 1. If it is already stopped, quit.
    if (!_running) {
        return get_instance();
    }

    // 2. Stop dependencies and then this instance.
    _yanpit_subscriber.stop();
    _running = false;
    _thread.join();

    return get_instance();
}

void YANPitRepresentation::operator()() {
    while (_running) {
        // TODO Do something...

        std::this_thread::sleep_for(std::chrono::milliseconds(_thread_sampling_rate));
    }
}

YANPitRepresentation::~YANPitRepresentation() {
    stop();
}