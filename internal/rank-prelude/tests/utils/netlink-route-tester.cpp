#include <cstring>
#include <ctime>
#include <iostream>

#include <unistd.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <iproute2/bpf_elf.h>

const char *rt_addr_n2a_r(int af, int len,
                          const void *addr, char *buf, int buflen)
{
    switch (af) {
        case AF_INET:
        case AF_INET6:
            return inet_ntop(af, addr, buf, buflen);
        case AF_BRIDGE:
        {
            union sock_addr {
                struct sockaddr sa;
                struct sockaddr_in sin;
                struct sockaddr_in6 sin6;
            };
            const auto* sa = (sock_addr*) addr;

            switch (sa->sa.sa_family) {
                case AF_INET:
                    return inet_ntop(AF_INET, &sa->sin.sin_addr,
                                     buf, buflen);
                case AF_INET6:
                    return inet_ntop(AF_INET6, &sa->sin6.sin6_addr,
                                     buf, buflen);
            }
        }
            /* fallthrough */
        default:
            return "???";
    }
}

const char *format_host(int af, int len, const void *addr)
{
    static char buf[256];

    return rt_addr_n2a_r(af, len, addr, buf, 256);
}

void print_arp_table() {
    struct sockaddr_nl socket_address;

    int netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (netlink_socket < 0) {
        std::cerr << "Something happened disallowing the netlink socket to be opened..." << std::endl;
        std::exit(1);
    }

    memset(&socket_address, 0, sizeof(socket_address));

    socket_address.nl_family = AF_NETLINK;
    socket_address.nl_pid = getpid();

    if (bind(netlink_socket, (struct sockaddr*)&socket_address, sizeof(socket_address)) < 0) {
        std::cerr << "An error occurred while attempting to bind to netlink socket. Quitting..." << std::endl;
        close(netlink_socket);
        std::exit(2);
    }

    struct {
        struct nlmsghdr header;
        struct ndmsg message;
    } request;

    request.header.nlmsg_type = RTM_GETNEIGH;
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    request.header.nlmsg_len = sizeof(request);
    request.header.nlmsg_seq = time(nullptr);

    ssize_t sent_bytes = send(netlink_socket, &request, sizeof(request), 0);
    if (sent_bytes < 0) {
        std::cerr << "An error occurred while attempting to send a request to netlink." << std::endl;
        std::exit(9);
    }

    char response_buffer[8192];
    ssize_t response_length = recv(netlink_socket, response_buffer, sizeof(response_buffer), 0);
    if (response_length < 0) {
        std::cerr << "Something weird has happened and nothing was received. Maybe the request is badly written?" << std::endl;
        std::exit(9);
    }

    // FROM HERE

    auto* response = (struct nlmsghdr*) response_buffer;
    while (NLMSG_OK(response, response_length)) {
        if (response->nlmsg_type == NLMSG_ERROR) {
            std::cerr << "Netlink error during parse of response message." << std::endl;
            std::exit(10);
        }

        // Parse ARP entry from netlink message.
        struct ndmsg* neighbor_discovery_message;
        struct rtattr* attribute;
        int attribute_length;

        neighbor_discovery_message = (struct ndmsg*) NLMSG_DATA(response);

        if (neighbor_discovery_message->ndm_family == AF_INET and (neighbor_discovery_message->ndm_state == NUD_REACHABLE or neighbor_discovery_message->ndm_state == NUD_STALE)) {
            attribute = (struct rtattr*) RTM_RTA(neighbor_discovery_message);
            attribute_length = response->nlmsg_len;

            // Loop through the routing attributes.
            while (RTA_OK(attribute, attribute_length)) {
                switch (attribute->rta_type) {
                    case NDA_DST: {   // This is a destination address.
                        const char *destination;
                        int family = neighbor_discovery_message->ndm_family;

                        if (family == AF_BRIDGE) {
                            if (RTA_PAYLOAD(attribute) == sizeof(struct in6_addr)) {
                                family = AF_INET6;
                            } else {
                                family = AF_INET;
                            }
                        }

                        destination = format_host(family, RTA_PAYLOAD(attribute), RTA_DATA(attribute));
                        std::cout << destination << " -> ";
                    }
                        break;
                    case NDA_LLADDR:
                        std::cout << std::hex << (int) ((unsigned char *) RTA_DATA(attribute))[0] << ":" <<
                                  std::hex << (int) ((unsigned char *) RTA_DATA(attribute))[1] << ":" <<
                                  std::hex << (int) ((unsigned char *) RTA_DATA(attribute))[2] << ":" <<
                                  std::hex << (int) ((unsigned char *) RTA_DATA(attribute))[3] << ":" <<
                                  std::hex << (int) ((unsigned char *) RTA_DATA(attribute))[4] << ":" <<
                                  std::hex << (int) ((unsigned char *) RTA_DATA(attribute))[5] << " ";
                        std::cout << (neighbor_discovery_message->ndm_state == NUD_STALE ? "(stale)" : "(reachable)") << std::endl;
                        break;
                }

                attribute = RTA_NEXT(attribute, attribute_length);
            }
        }
        response = NLMSG_NEXT(response, response_length);
    }

    // TO HERE

    close(netlink_socket);
}

int main() {
    print_arp_table();

    return 0;
}