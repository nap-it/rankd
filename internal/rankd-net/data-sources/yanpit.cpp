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

YanpitNetwork::YanpitNetwork() {
    _connection->execute();
    snap();
}

void YanpitNetwork::snap() {
    _interfaces = _connection->interfaces_information_as_map();
}

rapidjson::Document YanpitNetwork::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value.
    rapidjson::Value value;

    // Iterate through all the interfaces and print them.
    for (const auto& [name, interface] : _interfaces) {
        value.SetObject();
        json_document.AddMember(rapidjson::GenericStringRef(name.c_str()), value, allocator);
        value.SetString(rapidjson::GenericStringRef(interface.address().c_str()));
        json_document[name.c_str()].AddMember("mac-address", value, allocator);
        value.SetArray();
        json_document[name.c_str()].AddMember("ipv4-addresses", value, allocator);
        for (const auto& address : interface.ip_4_addresses()) {
            value.SetString(rapidjson::GenericStringRef(address.c_str()));
            json_document[name.c_str()]["ipv4-addresses"].PushBack(rapidjson::GenericStringRef(address.c_str()), allocator);
        }
        value.SetArray();
        json_document[name.c_str()].AddMember("ipv6-addresses", value, allocator);
        for (const auto& address : interface.ip_6_addresses()) {
            value.SetString(rapidjson::GenericStringRef(address.c_str()));
            json_document[name.c_str()]["ipv6-addresses"].PushBack(rapidjson::GenericStringRef(address.c_str()), allocator);
        }
        value.SetUint(interface.interface_index());
        json_document[name.c_str()].AddMember("index", value, allocator);
        value.SetUint(interface.enslaved_interface_index());
        json_document[name.c_str()].AddMember("enslaved-index", value, allocator);
        value.SetUint(interface.mtu());
        json_document[name.c_str()].AddMember("mtu", value, allocator);
        value.SetString(rapidjson::GenericStringRef(interface.operation_state().c_str()));
        json_document[name.c_str()].AddMember("operation-state", value, allocator);
        value.SetUint(interface.speed());
        json_document[name.c_str()].AddMember("speed", value, allocator);
        value.SetUint(interface.rx_bytes());
        json_document[name.c_str()].AddMember("rx_bytes", value, allocator);
        value.SetUint(interface.tx_bytes());
        json_document[name.c_str()].AddMember("tx_bytes", value, allocator);
        value.SetUint(interface.rx_packets());
        json_document[name.c_str()].AddMember("rx_packets", value, allocator);
        value.SetUint(interface.tx_packets());
        json_document[name.c_str()].AddMember("tx_packets", value, allocator);
        value.SetUint(interface.rx_errors());
        json_document[name.c_str()].AddMember("rx_errors", value, allocator);
        value.SetUint(interface.tx_errors());
        json_document[name.c_str()].AddMember("tx_errors", value, allocator);
        value.SetUint(interface.rx_dropped());
        json_document[name.c_str()].AddMember("rx_dropped", value, allocator);
        value.SetUint(interface.tx_dropped());
        json_document[name.c_str()].AddMember("tx_dropped", value, allocator);
    }

    // Return the JSON document back.
    return json_document;
}

std::ostream& operator<<(std::ostream& os, const YanpitNetwork& network) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    auto json_document = network.json();
    json_document.Accept(writer);

    os << buffer.GetString();

    return os;
}