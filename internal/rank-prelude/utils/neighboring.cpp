#include "utils/neighboring.h"

/* Adapted from iproute2 project */
int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len) {
    unsigned short type;

    memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
    while (RTA_OK(rta, len)) {
        type = rta->rta_type & ~0;
        if ((type <= max) && (!tb[type])) {
            tb[type] = rta;
        }
        rta = RTA_NEXT(rta, len);
    }
    if (len) {
        throw std::exception();
        fprintf(stderr, "!!!Deficit %d, rta_len=%d\n",
                len, rta->rta_len);
    }
    return 0;
}

/* Adapted from iproute2. */
static inline int rtm_get_table(struct rtmsg *r, struct rtattr **tb) {
    __u32 table = r->rtm_table;

    if (tb[RTA_TABLE])
        table = *((__u32 *) RTA_DATA(tb[RTA_TABLE]));
    return table;
}

bool is_within_range(const std::vector<uint8_t> &tester, const std::vector<uint8_t> &tested,
                     unsigned char number_of_equal_bits) {
    assert(tester.size() == 4 or tester.size() == 16);
    assert(tested.size() == 4 or tester.size() == 16);
    assert(tester.size() == tested.size());
    assert(tester.size() == 4 and (number_of_equal_bits >= 0 and number_of_equal_bits <= 32));
    assert(tester.size() == 16 and (number_of_equal_bits >= 0 and number_of_equal_bits <= 128));

    // Convert the tester and tested vectors to bitsets.
    std::bitset<128> tester_bits, tested_bits;
    for (size_t i = 0; i < tester.size(); i++) {
        tester_bits |= std::bitset<128>(tester[i]) << (8 * (tester.size() - i - 1));
        tested_bits |= std::bitset<128>(tested[i]) << (8 * (tester.size() - i - 1));
    }

    // Compare the first number_of_equal_bits of the bitsets.
    for (size_t i = 0; i < number_of_equal_bits; i++) {
        if (tester_bits[i] != tested_bits[i]) {
            return false;
        }
    }

    return true;
}

std::vector<std::vector<uint8_t>> get_connections_to(const std::vector<uint8_t> &target) {
    std::vector<std::vector<uint8_t>> addresses{};

    // Get the type of target address.
    switch (target.size()) {
        case 4:
        case 16: {
            // If the address is IP, check for routes in IP.

            // Create base structures for the netlink socket to be placed.
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

            // Create request message to netlink for routing dump request.
            struct {
                struct nlmsghdr header;
                struct rtmsg message;
            } request;
            request.header.nlmsg_type = RTM_GETROUTE;
            request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
            request.header.nlmsg_len = sizeof(request);
            request.header.nlmsg_seq = time(nullptr);
            if (target.size() == 4) {
                request.message.rtm_family = AF_INET;
            } else if (target.size() == 16) {
                request.message.rtm_family = AF_INET6;
            } else {
                throw std::exception();
            }

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
            auto *response = (struct nlmsghdr *) response_buffer;
            while (NLMSG_OK(response, received_bytes)) {
                auto *content = (struct rtmsg *) NLMSG_DATA(response);
                int content_length = response->nlmsg_len;

                // Access attribute details as a table.
                struct rtattr *table[RTA_MAX + 1];
                int table_item;

                // Decrement the current item length in the base length.
                content_length -= NLMSG_LENGTH(sizeof(*content));
                if (content_length < 0) {
                    throw std::exception();
                }

                // Parse attributes to table.
                parse_rtattr(table, RTA_MAX, RTM_RTA(content), content_length);
                table_item = rtm_get_table(content, table);

                // Check if is a gateway or not.
                if (table[RTA_GATEWAY]) {
                    std::vector<uint8_t> possible_destination;
                    if (target.size() == 4) {
                        possible_destination = std::vector({
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[0],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[1],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[2],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[3]
                                                           });
                    } else if (target.size() == 16) {
                        possible_destination = std::vector({
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[0],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[1],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[2],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[3],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[4],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[5],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[6],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[7],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[8],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[9],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[10],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[11],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[12],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[13],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[14],
                                                                   ((unsigned char *) RTA_DATA(table[RTA_GATEWAY]))[15]
                                                           });
                    }

                    if (table[RTA_DST]) {
                        std::vector<uint8_t> route_address;
                        if (target.size() == 4) {
                            route_address = std::vector({
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[0],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[1],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[2],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[3]
                                                        });
                        } else if (target.size() == 16) {
                            route_address = std::vector({
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[0],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[1],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[2],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[3],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[4],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[5],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[6],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[7],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[8],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[9],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[10],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[11],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[12],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[13],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[14],
                                                                ((unsigned char *) RTA_DATA(table[RTA_DST]))[15]
                                                        });
                        }
                        auto mask = content->rtm_dst_len;

                        if (is_within_range(possible_destination, route_address, mask)) {
                            addresses.push_back(possible_destination);
                        }
                    }
                }
                response = NLMSG_NEXT(response, received_bytes);
            }

            close(netlink_socket);
        }
            break;
        case 6: {
            // If the address is MAC, check for forwarding paths in Ethernet.

            // Create base structures for the netlink socket to be placed.
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
            auto *response = (struct nlmsghdr *) response_buffer;
            while (NLMSG_OK(response, received_bytes)) {
                struct ndmsg *nd_message;
                struct rtattr *attribute;
                int attribute_length;

                // Parse content as a proper neighbor discovery message.
                nd_message = (struct ndmsg *) NLMSG_DATA(response);

                // Check that we are only dealing with IP, reachable and stale entries of the ARP table.
                if (nd_message->ndm_family == AF_INET and
                    (nd_message->ndm_state == NUD_REACHABLE or nd_message->ndm_state == NUD_STALE)) {
                    attribute = (struct rtattr *) RTM_RTA(nd_message);
                    attribute_length = response->nlmsg_len;

                    // Loop through the routing attributes.
                    std::vector<uint8_t> possible_destination{};
                    std::vector<uint8_t> possible_mac_address{};

                    while (RTA_OK(attribute, attribute_length) and possible_mac_address.empty() and
                           possible_destination.empty()) {
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

                                if (family == AF_INET) {
                                    possible_destination = {
                                            ((unsigned char *) RTA_DATA(attribute))[0],
                                            ((unsigned char *) RTA_DATA(attribute))[1],
                                            ((unsigned char *) RTA_DATA(attribute))[2],
                                            ((unsigned char *) RTA_DATA(attribute))[3]
                                    };
                                } else if (family == AF_INET6) {
                                    possible_destination = {
                                            ((unsigned char *) RTA_DATA(attribute))[0],
                                            ((unsigned char *) RTA_DATA(attribute))[1],
                                            ((unsigned char *) RTA_DATA(attribute))[2],
                                            ((unsigned char *) RTA_DATA(attribute))[3],
                                            ((unsigned char *) RTA_DATA(attribute))[4],
                                            ((unsigned char *) RTA_DATA(attribute))[5],
                                            ((unsigned char *) RTA_DATA(attribute))[6],
                                            ((unsigned char *) RTA_DATA(attribute))[7],
                                            ((unsigned char *) RTA_DATA(attribute))[8],
                                            ((unsigned char *) RTA_DATA(attribute))[9],
                                            ((unsigned char *) RTA_DATA(attribute))[10],
                                            ((unsigned char *) RTA_DATA(attribute))[11],
                                            ((unsigned char *) RTA_DATA(attribute))[12],
                                            ((unsigned char *) RTA_DATA(attribute))[13],
                                            ((unsigned char *) RTA_DATA(attribute))[14],
                                            ((unsigned char *) RTA_DATA(attribute))[15]
                                    };
                                }
                            }
                            case NDA_LLADDR:
                                possible_mac_address = {
                                        ((unsigned char *) RTA_DATA(attribute))[0],
                                        ((unsigned char *) RTA_DATA(attribute))[1],
                                        ((unsigned char *) RTA_DATA(attribute))[2],
                                        ((unsigned char *) RTA_DATA(attribute))[3],
                                        ((unsigned char *) RTA_DATA(attribute))[4],
                                        ((unsigned char *) RTA_DATA(attribute))[5]
                                };

                                if (target != possible_mac_address) {
                                    possible_destination.clear();
                                    possible_mac_address.clear();
                                }

                                break;
                        }

                        attribute = RTA_NEXT(attribute, attribute_length);
                    }

                    if (not possible_mac_address.empty() and not possible_destination.empty()) {
                        addresses.push_back(possible_destination);
                        break;
                    }

                }
                response = NLMSG_NEXT(response, received_bytes);
            }

            close(netlink_socket);
        }

            break;
    }

    return addresses;
}

unsigned int connections_cardinal(const std::vector<std::vector<uint8_t>> &connections) {
    return connections.size();
}
