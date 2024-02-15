#include "data-sources/interfaces.h"

NetworkDevices::NetworkDevices() {
    _devices = std::map<int, NetworkDevice>();

    snap();
}

void NetworkDevices::snap() {
    _devices.clear();

    struct sockaddr_nl binding_address{};
    int netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (netlink_socket < 0) {
        throw std::ios_base::failure("Netlink socket could not be opened in NetworkDevices (First phase).");
    }
    memset(&binding_address, 0, sizeof(binding_address));
    binding_address.nl_family = AF_NETLINK;
    binding_address.nl_pid = 0;

    // Bind netlink socket and address.
    if (bind(netlink_socket, (struct sockaddr*) &binding_address, sizeof(binding_address)) < 0) {
        close(netlink_socket);
        throw std::ios_base::failure("Netlink socket could not be bound in NetworkDevices.");
    }

    // Create request message to netlink for interface dump request.
    struct {
        struct nlmsghdr header;
        struct ifinfomsg message;
    } request{};
    request.header.nlmsg_type = RTM_GETLINK;
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    request.header.nlmsg_len = sizeof(request);
    request.header.nlmsg_seq = time(nullptr);
    request.message.ifi_family = AF_UNSPEC;

    // Place request to kernel via netlink procedures.
    ssize_t sent_bytes = send(netlink_socket, &request, sizeof(request), 0);
    if (sent_bytes < 0) {
        close(netlink_socket);
        throw std::ios_base::failure("Netlink socket could not be used to send data in NetworkDevices (First phase).");
    }

    // Receive data from kernel's netlink procedure.
    char response_buffer[8192];

    bool receiving = true;
    while (receiving) {
        ssize_t received_bytes = recv(netlink_socket, response_buffer, sizeof(response_buffer), 0);
        if (received_bytes < 0) {
            close(netlink_socket);
            throw std::ios_base::failure("Netlink socket could not be used to send data in NetworkDevices (First phase).");
        }

        auto *response = (struct nlmsghdr *) response_buffer;
        while (NLMSG_OK(response, received_bytes)) {
            if (response->nlmsg_type == NLMSG_DONE) {
                receiving = false;
                break;
            }

            auto *content = (struct ifinfomsg *) NLMSG_DATA(response);
            int content_length = response->nlmsg_len;

            // Create a new network device.
            NetworkDevice device;
            device.index = content->ifi_index;
            device.flags = content->ifi_flags;

            auto* attribute = (struct rtattr*) IFLA_RTA(content);
            int attribute_length = IFLA_PAYLOAD(response);
            for (; RTA_OK(attribute, attribute_length); attribute = RTA_NEXT(attribute, attribute_length)) {
                switch (attribute->rta_type) {
                    case IFLA_IFNAME:
                        device.name = std::string((char*) RTA_DATA(attribute));
                        break;
                    case IFLA_ADDRESS:
                        device.l2_address = mac_to_string((unsigned char*) RTA_DATA(attribute));
                        break;
                    case IFLA_BROADCAST:
                        device.l2_broadcast_address = mac_to_string((unsigned char*) RTA_DATA(attribute));
                        break;
                    case IFLA_MTU:
                        device.mtu = *(int*) RTA_DATA(attribute);
                        break;
                    case IFLA_LINK:
                        device.link_type = *(int*) RTA_DATA(attribute);
                        break;
                    case IFLA_QDISC:
                        device.qdisc = std::string((char*) RTA_DATA(attribute));
                        break;
                    case IFLA_STATS:
                        device.statistics = *(DeviceStatistics*) RTA_DATA(attribute);
                        break;
                }
            }

            _devices[device.index] = device;
            response = NLMSG_NEXT(response, received_bytes);
        }
    }

    close(netlink_socket);

    netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (netlink_socket < 0) {
        throw std::ios_base::failure("Netlink socket could not be opened in NetworkDevices (Second phase).");
    }

    // Construct message to request interface addresses.
    struct {
        struct nlmsghdr header;
        struct rtgenmsg message;
    } int_request;
    int_request.header.nlmsg_type = RTM_GETADDR;
    int_request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    int_request.header.nlmsg_len = NLMSG_LENGTH(sizeof(request.message));
    int_request.header.nlmsg_seq = time(nullptr);
    int_request.message.rtgen_family = AF_UNSPEC;

    // Place request to kernel via netlink procedures.
    sent_bytes = send(netlink_socket, &int_request, int_request.header.nlmsg_len, 0);
    if (sent_bytes < 0) {
        close(netlink_socket);
        throw std::ios_base::failure("Netlink socket could not be used to send data in NetworkDevices (Second phase).");
    }

    receiving = true;
    memset(&response_buffer, 0, sizeof(response_buffer));
    while (receiving) {
        ssize_t received_bytes = recv(netlink_socket, response_buffer, sizeof(response_buffer), 0); // FIXME Blocking here...
        if (received_bytes < 0) {
            close(netlink_socket);
            throw std::ios_base::failure("Netlink socket could not be used to send data in NetworkDevices (Second phase).");
        }

        auto *response = (struct nlmsghdr *) response_buffer;
        while (NLMSG_OK(response, received_bytes)) {
            if (response->nlmsg_type == NLMSG_DONE) {
                receiving = false;
                break;
            }

            auto *content = (struct ifaddrmsg *) NLMSG_DATA(response);
            int content_length = response->nlmsg_len;

            int interface_index = content->ifa_index;

            auto* attribute = (struct rtattr*) IFA_RTA(content);
            int attribute_length = IFA_PAYLOAD(response);
            for (; RTA_OK(attribute, attribute_length); attribute = RTA_NEXT(attribute, attribute_length)) {
                switch (attribute->rta_type) {
                    case IFA_ADDRESS:
                        _devices[interface_index].addresses.emplace_back(content->ifa_family, ip_to_string(content->ifa_family, RTA_DATA(attribute)));
                        break;
                }
            }

            response = NLMSG_NEXT(response, received_bytes);
        }
    }

    close(netlink_socket);
}

void NetworkDevices::enable_json_output() {
    _json_formatted_output = true;
}

void NetworkDevices::disable_json_output() {
    _json_formatted_output = false;
}

rapidjson::Document NetworkDevices::json() const {
    // Create a JSON Document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value.
    rapidjson::Value value;

    for (const auto& [index, device] : _devices) {
        value.SetObject();
        json_document.AddMember(rapidjson::GenericStringRef(device.name.c_str()), value, allocator);

        value.SetInt(device.flags);
        json_document[device.name.c_str()].AddMember("flags", value, allocator);
        value.SetString(rapidjson::GenericStringRef(device.l2_address.c_str()));
        json_document[device.name.c_str()].AddMember("l2-address", value, allocator);
        value.SetString(rapidjson::GenericStringRef(device.l2_broadcast_address.c_str()));
        json_document[device.name.c_str()].AddMember("l2-broadcast", value, allocator);
        value.SetArray();
        json_document[device.name.c_str()].AddMember("addresses", value, allocator);
        for (const auto& [family, address] : device.addresses) {
            value.SetObject();
            rapidjson::Value subvalue;
            subvalue.SetInt(family);
            value.AddMember("family", subvalue, allocator);
            subvalue.SetString(rapidjson::GenericStringRef(address.c_str()));
            value.AddMember("address", subvalue, allocator);
            json_document[device.name.c_str()]["addresses"].PushBack(value, allocator);
        }
        value.SetInt(device.mtu);
        json_document[device.name.c_str()].AddMember("mtu", value, allocator);
        value.SetInt(device.link_type);
        json_document[device.name.c_str()].AddMember("link-type", value, allocator);
        value.SetString(rapidjson::GenericStringRef(device.qdisc.c_str()));
        json_document[device.name.c_str()].AddMember("qdisc", value, allocator);

        value.SetObject();
        json_document[device.name.c_str()].AddMember("statistics", value, allocator);
        value.SetUint(device.statistics.rx_packets);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_packets", value, allocator);
        value.SetUint(device.statistics.tx_packets);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_packets", value, allocator);
        value.SetUint(device.statistics.rx_bytes);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_bytes", value, allocator);
        value.SetUint(device.statistics.tx_bytes);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_bytes", value, allocator);
        value.SetUint(device.statistics.rx_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_errors", value, allocator);
        value.SetUint(device.statistics.tx_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_errors", value, allocator);
        value.SetUint(device.statistics.rx_dropped);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_dropped", value, allocator);
        value.SetUint(device.statistics.tx_dropped);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_dropped", value, allocator);
        value.SetUint(device.statistics.multicast);
        json_document[device.name.c_str()]["statistics"].AddMember("multicast", value, allocator);
        value.SetUint(device.statistics.collisions);
        json_document[device.name.c_str()]["statistics"].AddMember("collisions", value, allocator);
        value.SetUint(device.statistics.rx_length_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_length_errors", value, allocator);
        value.SetUint(device.statistics.rx_over_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_over_errors", value, allocator);
        value.SetUint(device.statistics.rx_crc_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_crc_errors", value, allocator);
        value.SetUint(device.statistics.rx_frame_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_frame_errors", value, allocator);
        value.SetUint(device.statistics.rx_fifo_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_fifo_errors", value, allocator);
        value.SetUint(device.statistics.rx_missed_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_missed_errors", value, allocator);
        value.SetUint(device.statistics.tx_aborted_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_aborted_errors", value, allocator);
        value.SetUint(device.statistics.tx_carrier_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_carrier_errors", value, allocator);
        value.SetUint(device.statistics.tx_fifo_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_fifo_errors", value, allocator);
        value.SetUint(device.statistics.tx_heartbeat_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_heartbeat_errors", value, allocator);
        value.SetUint(device.statistics.tx_window_errors);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_window_errors", value, allocator);
        value.SetUint(device.statistics.rx_compressed);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_compressed", value, allocator);
        value.SetUint(device.statistics.tx_compressed);
        json_document[device.name.c_str()]["statistics"].AddMember("tx_compressed", value, allocator);
        value.SetUint(device.statistics.rx_nohandler);
        json_document[device.name.c_str()]["statistics"].AddMember("rx_nohandler", value, allocator);
    }

    return json_document;
}

std::ostream& operator<<(std::ostream& os, const NetworkDevices& devices) {
    if (devices._json_formatted_output) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        auto json_document = devices.json();
        json_document.Accept(writer);

        os << buffer.GetString();
    } else {
        for (const auto& [index, interface] : devices._devices) {
            os << "(" << index << ") " << interface.name << ": " << "flags=" << interface.flags << "  mtu " << interface.mtu << "\n";
            if (not interface.l2_address.empty()) {
                os << "\t" << "link " << interface.l2_address << " broadcast " << interface.l2_broadcast_address << " link-type " << interface.link_type << "\n";
            }
            if (not interface.addresses.empty()) {
                for (const auto& [family, address] : interface.addresses) {
                    if (family == AF_INET) {
                        os << "\t" << "inet " << address << "\n";
                    } else if (family == AF_INET6) {
                        os << "\t" << "inet6 " << address << "\n";
                    }
                }
            }
            os << "\t" << "qdisc " << interface.qdisc << "\n";
            os << std::endl;
        }
    }

    return os;
}