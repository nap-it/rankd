#include "utils/addresses.h"

std::string mac_to_string(unsigned char* mac) {
    char buffer[18];
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(buffer);
}

std::string ip_to_string(int family, void* addr) {
    char buffer[INET6_ADDRSTRLEN];
    inet_ntop(family, addr, buffer, sizeof(buffer));
    return std::string(buffer);
}

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

std::string get_interface_name(int index) {
    // Create a socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return "";
    }

    // Create an ifreq structure
    struct ifreq ifr;
    ifr.ifr_ifindex = index;

    // Use ioctl to get the interface name
    int ret = ioctl(sock, SIOCGIFNAME, &ifr);
    if (ret < 0) {
        close(sock);
        return "";
    }

    // Close the socket
    close(sock);

    // Return the interface name as a std::string
    return std::string(ifr.ifr_name);
}