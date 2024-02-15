#include "data-sources/routes.h"

NetworkRoutes::NetworkRoutes() {
    _routes = std::vector<NetworkRoute>();

    snap();
}

void NetworkRoutes::snap() {
    _routes.clear();

    struct sockaddr_nl binding_address{};
    int netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (netlink_socket < 0) {
        throw std::ios_base::failure("Netlink socket could not be opened in NetworkRoutes.");
    }
    memset(&binding_address, 0, sizeof(binding_address));
    binding_address.nl_family = AF_NETLINK;
    binding_address.nl_pid = 0;

    // Bind netlink socket and address.
    if (bind(netlink_socket, (struct sockaddr *) &binding_address, sizeof(binding_address)) < 0) {
        close(netlink_socket);
        throw std::ios_base::failure("Netlink socket could not be bound in NetworkRoutes.");
    }

    // Create request message to netlink for routing dump request.
    struct {
        struct nlmsghdr header;
        struct rtmsg message;
    } request{};
    request.header.nlmsg_type = RTM_GETROUTE;
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    request.header.nlmsg_len = sizeof(request);
    request.header.nlmsg_seq = time(nullptr);
    request.message.rtm_family = AF_UNSPEC;

    // Place request to kernel via netlink procedures.
    ssize_t sent_bytes = send(netlink_socket, &request, sizeof(request), 0);
    if (sent_bytes < 0) {
        close(netlink_socket);
        throw std::ios_base::failure("Netlink socket could not be used to send data in NetworkRoutes.");
    }

    // Receive data from kernel's netlink procedure.
    char response_buffer[8192];
    ssize_t received_bytes = recv(netlink_socket, response_buffer, sizeof(response_buffer), 0);
    if (received_bytes < 0) {
        close(netlink_socket);
        throw std::ios_base::failure("Netlink socket could not be used to receive data in NetworkRoutes.");
    }

    std::cerr << "The response was " << received_bytes << std::endl;

    // Parse the response from the kernel as a netlink message header for further details.
    auto *response = (struct nlmsghdr *) response_buffer;
    while (NLMSG_OK(response, received_bytes)) {
        auto *content = (struct rtmsg *) NLMSG_DATA(response);
        int content_length = response->nlmsg_len;

        // Access attribute details as a table.
        struct rtattr *table[RTA_MAX + 1];

        // Decrement the current item length in the base length.
        content_length -= NLMSG_LENGTH(sizeof(*content));
        if (content_length < 0) {
            close(netlink_socket);
            std::cerr << "Wrong message length in NetworkRoutes. Routes length is " << _routes.size() << std::endl; // TODO Handle this error.
            return;
        }

        // Parse attributes to table.
        parse_rtattr(table, RTA_MAX, RTM_RTA(content), content_length);

        NetworkRoute route;
        route.family = content->rtm_family;
        route.destination_length = content->rtm_dst_len;
        route.source_length = content->rtm_src_len;
        route.tos = content->rtm_tos;
        route.table_id = content->rtm_table;

        switch (content->rtm_type) {
            case RTN_UNICAST:
                route.type = "unicast";
                break;
            case RTN_LOCAL:
                route.type = "local";
                break;
            case RTN_BROADCAST:
                route.type = "broadcast";
                break;
            case RTN_ANYCAST:
                route.type = "anycast";
                break;
            case RTN_MULTICAST:
                route.type = "multicast";
                break;
            case RTN_BLACKHOLE:
                route.type = "blackhole";
                break;
            case RTN_UNREACHABLE:
                route.type = "unreachable";
                break;
            case RTN_PROHIBIT:
                route.type = "prohibit";
                break;
            case RTN_THROW:
                route.type = "throw";
                break;
            case RTN_NAT:
                route.type = "nat";
                break;
            case RTN_XRESOLVE:
                route.type = "xresolve";
                break;
            default:
                route.type = "";
                break;
        }

        switch (content->rtm_protocol) {
            case RTPROT_REDIRECT:
                route.route_protocol = "redirect";
                break;
            case RTPROT_KERNEL:
                route.route_protocol = "kernel";
                break;
            case RTPROT_BOOT:
                route.route_protocol = "boot";
                break;
            case RTPROT_STATIC:
                route.route_protocol = "static";
                break;
            case RTPROT_GATED:
                route.route_protocol = "GateD";
                break;
            case RTPROT_RA:
                route.route_protocol = "RDISC/ND router advertisements";
                break;
            case RTPROT_MRT:
                route.route_protocol = "Merit MRT";
                break;
            case RTPROT_ZEBRA:
                route.route_protocol = "Zebra";
                break;
            case RTPROT_BIRD:
                route.route_protocol = "BIRD";
                break;
            case RTPROT_DNROUTED:
                route.route_protocol = "DECnet routing daemon";
                break;
            case RTPROT_XORP:
                route.route_protocol = "XORP";
                break;
            case RTPROT_NTK:
                route.route_protocol = "Netsukuku";
                break;
            case RTPROT_DHCP:
                route.route_protocol = "DHCP client";
                break;
            case RTPROT_MROUTED:
                route.route_protocol = "Multicast daemon";
                break;
            case RTPROT_BABEL:
                route.route_protocol = "Babel daemon";
                break;
            case RTPROT_BGP:
                route.route_protocol = "BGP";
                break;
            case RTPROT_ISIS:
                route.route_protocol = "IS-IS";
                break;
            case RTPROT_OSPF:
                route.route_protocol = "OSPF";
                break;
            case RTPROT_RIP:
                route.route_protocol = "RIP";
                break;
            case RTPROT_EIGRP:
                route.route_protocol = "EIGRP";
                break;
            default:
                route.route_protocol = "";
                break;
        }

        switch (content->rtm_scope) {
            case RT_SCOPE_UNIVERSE:
                route.scope = "universe";
                break;
            case RT_SCOPE_SITE:
                route.scope = "site";
                break;
            case RT_SCOPE_LINK:
                route.scope = "link";
                break;
            case RT_SCOPE_HOST:
                route.scope = "host";
                break;
            case RT_SCOPE_NOWHERE:
                route.scope = "nowhere";
                break;
            default:
                route.scope = "";
                break;
        }

        switch (content->rtm_flags) {
            case RTM_F_NOTIFY:
                route.flags = "notify";
                break;
            case RTM_F_CLONED:
                route.flags = "cloned from another";
                break;
            case RTM_F_EQUALIZE:
                route.flags = "multipath equalizer";
                break;
            default:
                route.flags = "";
                break;
        }

        if (table[RTA_GATEWAY]) {
            route.gateway = ip_to_string(route.family, (unsigned char *) RTA_DATA(table[RTA_GATEWAY]));
        }

        if (table[RTA_DST]) {
            route.destination = ip_to_string(route.family, (unsigned char *) RTA_DATA(table[RTA_DST]));
        }

        if (table[RTA_SRC]) {
            route.source = ip_to_string(route.family, (unsigned char *) RTA_DATA(table[RTA_SRC]));
        }

        if (table[RTA_IIF]) {
            route.input_interface = get_interface_name(*(int *) RTA_DATA(table[RTA_IIF]));
        }

        if (table[RTA_OIF]) {
            route.output_interface = get_interface_name(*(int *) RTA_DATA(table[RTA_OIF]));
        }

        if (table[RTA_PRIORITY]) {
            route.priority = *(int *) RTA_DATA(table[RTA_PRIORITY]);
        }

        if (table[RTA_METRICS]) {
            route.metric = *(int *) RTA_DATA(table[RTA_METRICS]);
        }

        _routes.push_back(route);
        response = NLMSG_NEXT(response, received_bytes);
    }

    close(netlink_socket);
}

void NetworkRoutes::enable_json_output() {
    _json_formatted_output = true;
}

void NetworkRoutes::disable_json_output() {
    _json_formatted_output = false;
}

rapidjson::Document NetworkRoutes::json() const {
    // Create a JSON Document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value.
    rapidjson::Value value;

    value.SetArray();
    json_document.AddMember("routes", value, allocator);
    for (const auto& route: _routes) {
        value.SetObject();
        rapidjson::Value subvalue;
        subvalue.SetInt(route.family);
        value.AddMember("family", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(route.destination.c_str()));
        value.AddMember("destination", subvalue, allocator);
        subvalue.SetInt(route.destination_length);
        value.AddMember("destination-length", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(route.source.c_str()));
        value.AddMember("source", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(route.gateway.c_str()));
        value.AddMember("gateway", subvalue, allocator);
        subvalue.SetInt(route.source_length);
        value.AddMember("source-length", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(route.input_interface.c_str()));
        value.AddMember("input-interface", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(route.output_interface.c_str()));
        value.AddMember("output-interface", subvalue, allocator);
        subvalue.SetInt(route.tos);
        value.AddMember("tos", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(route.route_protocol.c_str()));
        value.AddMember("protocol", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(route.scope.c_str()));
        value.AddMember("scope", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(route.type.c_str()));
        value.AddMember("type", subvalue, allocator);
        subvalue.SetInt(route.priority);
        value.AddMember("priority", subvalue, allocator);
        subvalue.SetInt(route.metric);
        value.AddMember("metric", subvalue, allocator);
        subvalue.SetString(rapidjson::GenericStringRef(route.flags.c_str()));
        value.AddMember("flags", subvalue, allocator);
        json_document["routes"].PushBack(value, allocator);
    }

    return json_document;
}

std::ostream& operator<<(std::ostream& os, const NetworkRoutes& routes) {
    if (routes._json_formatted_output) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        auto json_document = routes.json();
        json_document.Accept(writer);

        os << buffer.GetString();
    } else {
        for (const auto& route : routes._routes) {
            if (route.gateway.empty()) {
                os << route.destination << "/" << route.destination_length << " -> " << route.output_interface << "\n";
            } else {
                os << "default -> " << route.gateway << " through " << route.output_interface << "\n";
            }
            if (not route.source.empty()) {
                os << "\t" << "source: " << route.source << "/" << route.source_length << "\n";
            }
            os << "\t" << "protocol: " << route.route_protocol << "\n";
            os << "\t" << "scope: " << route.scope << ", type: " << route.type << "\n";
            os << "\t" << "tos: " << route.tos << ", priority: " << route.priority << ", metric: " << route.metric << "\n";
            os << std::endl;
        }
    }

    return os;
}