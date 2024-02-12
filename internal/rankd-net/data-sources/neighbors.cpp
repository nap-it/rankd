#include "data-sources/neighbors.h"

NetworkNeighbors::NetworkNeighbors() {
    _neighbors = std::vector<NetworkNeighbor>();

    snap();
}

void NetworkNeighbors::snap() {
    struct sockaddr_nl binding_address;
    int netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (netlink_socket < 0) {
        throw std::exception();
    }
    memset(&binding_address, 0, sizeof(binding_address));
    binding_address.nl_family = AF_NETLINK;
    binding_address.nl_pid = getpid();

    // Bind netlink socket and address.
    if (bind(netlink_socket, (struct sockaddr *) &binding_address, sizeof(binding_address)) < 0) {
        throw std::exception();
    }

    // Create request message to netlink for neighbors dump request.
    struct {
        struct nlmsghdr header;
        struct ndmsg message;
    } request;
    request.header.nlmsg_type = RTM_GETNEIGH;
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    request.header.nlmsg_len = sizeof(request);
    request.header.nlmsg_seq = time(nullptr);

    // Place request to kernel via netlink procedures.
    ssize_t sent_bytes = send(netlink_socket, &request, sizeof(request), 0);
    if (sent_bytes < 0) {
        throw std::exception();
    }

    // Receive data from kernel's netlink procedure.
    char response_buffer[8192];
    ssize_t received_bytes = recv(netlink_socket, response_buffer, sizeof(response_buffer), 0);
    if (received_bytes < 0) {
        throw std::exception();
    }

    // Parse the response from the kernel as a netlink message header for further details.
    auto* response = (struct nlmsghdr*) response_buffer;
    while (NLMSG_OK(response, received_bytes)) {
        struct ndmsg* nd_message;
        struct rtattr* attribute;
        int attribute_length;

        // Parse content as a proper neighbor discovery message.
        nd_message = (struct ndmsg*) NLMSG_DATA(response);

        attribute = (struct rtattr*) RTM_RTA(nd_message);
        attribute_length = response->nlmsg_len;

        NetworkNeighbor neighbor;

        neighbor.family = nd_message->ndm_family;

        switch (nd_message->ndm_state) {
            case NUD_INCOMPLETE:
                neighbor.state = "incomplete";
                break;
            case NUD_REACHABLE:
                neighbor.state = "reachable";
                break;
            case NUD_STALE:
                neighbor.state = "stale";
                break;
            case NUD_DELAY:
                neighbor.state = "delay";
                break;
            case NUD_PROBE:
                neighbor.state = "probe";
                break;
            case NUD_FAILED:
                neighbor.state = "failed";
                break;
            case NUD_NOARP:
                neighbor.state = "no ARP";
                break;
            case NUD_PERMANENT:
                neighbor.state = "permanent";
                break;
            default:
                neighbor.state = "";
                break;
        }

        if ((nd_message->ndm_flags & NTF_PROXY) == nd_message->ndm_flags) {
            neighbor.flags.push_back("proxy");
        }
        if ((nd_message->ndm_flags & NTF_ROUTER) == nd_message->ndm_flags) {
            neighbor.flags.push_back("router");
        }

        while (RTA_OK(attribute, attribute_length)) {
            switch (attribute->rta_type) {
                case NDA_DST: {
                    int family = nd_message->ndm_family;
                    if (family == AF_BRIDGE) {
                        if (RTA_PAYLOAD(attribute) == sizeof(struct in6_addr)) {
                            family = AF_INET6;
                        } else {
                            family = AF_INET;
                        }
                    }
                    neighbor.l3_address = ip_to_string(family, (unsigned char*) RTA_DATA(attribute));
                }
                break;
                case NDA_LLADDR:
                    neighbor.l2_address = mac_to_string((unsigned char*) RTA_DATA(attribute));
                    break;
            }

            attribute = RTA_NEXT(attribute, attribute_length);
        }

        _neighbors.push_back(neighbor);

        response = NLMSG_NEXT(response, received_bytes);
    }

    close(netlink_socket);
}

rapidjson::Document NetworkNeighbors::json() const {
    // Create a JSON Document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value.
    rapidjson::Value value;

    value.SetArray();
    json_document.AddMember("neighbors", value, allocator);
    for (const auto& neighbor : _neighbors) {
        value.SetObject();
        rapidjson::Value subvalue;
        subvalue.SetInt(neighbor.family);
        value.AddMember("family", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(neighbor.state.c_str()));
        value.AddMember("state", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(neighbor.l2_address.c_str()));
        value.AddMember("mac", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(neighbor.l3_address.c_str()));
        value.AddMember("ip", subvalue, allocator);
        subvalue.SetArray();
        value.AddMember("flags", subvalue, allocator);

        for (const auto& flag : neighbor.flags) {
            rapidjson::Value subsubvalue;
            subsubvalue.SetString(rapidjson::GenericStringRef(flag.c_str()));
            value["flags"].PushBack(subsubvalue, allocator);
        }
        json_document["neighbors"].PushBack(value, allocator);
    }

    return json_document;
}

std::ostream& operator<<(std::ostream& os, const NetworkNeighbors& neighbors) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    auto json_document = neighbors.json();
    json_document.Accept(writer);

    os << buffer.GetString();

    return os;
}
