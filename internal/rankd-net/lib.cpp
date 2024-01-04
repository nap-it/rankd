#include "net-lib.h"

Network::Network() {
    _connection->execute();
    snap();
}

void Network::snap() {
    _interfaces = _connection->interfaces_information_as_map();
}

rapidjson::Document Network::json() const {
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

std::ostream& operator<<(std::ostream& os, const Network& network) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    auto json_document = network.json();
    json_document.Accept(writer);

    os << buffer.GetString();

    return os;
}
