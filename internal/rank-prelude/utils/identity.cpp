#include "utils/identity.h"

bool is_me(std::array<uint8_t, 4>& ipv4_address) {
#if defined(__linux__) || defined(__unix__)
    // Get all current network interfaces from the system.
    struct ifaddrs *addresses;
    getifaddrs(&addresses);

    for (struct ifaddrs *address = addresses; address != nullptr; address = address->ifa_next) {
        if (address->ifa_addr && (address->ifa_addr->sa_family == AF_PACKET || address->ifa_addr->sa_family == AF_PPPOX)) {
            // In address->ifa_name the name of the current interface can be accessed.

            // Create a socket to communicate with the kernel asking for the IP address.
            int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
            struct ifreq interface_request{};
            strcpy(interface_request.ifr_name, address->ifa_name);
            ioctl(socket_fd, SIOCGIFADDR, &interface_request);
            close(socket_fd);

            std::array<uint8_t, 4> ip{};
            auto raw_ip = ((sockaddr_in*) &interface_request.ifr_addr)->sin_addr;
            ip[0] = raw_ip.s_addr & 0xFF;
            ip[1] = (raw_ip.s_addr >> 8) & 0xFF;
            ip[2] = (raw_ip.s_addr >> 16) & 0xFF;
            ip[3] = (raw_ip.s_addr >> 24) & 0xFF;

            if (ip == ipv4_address) {
                return true;
            }
        }
    }
#endif

    return false;
}

bool is_me(std::array<uint8_t, 6>& mac_address) {
#if defined(__linux__) || defined(__unix__)
    // Get all current network interfaces from the system.
    struct ifaddrs *addresses;
    getifaddrs(&addresses);

    for (struct ifaddrs *address = addresses; address != nullptr; address = address->ifa_next) {
        if (address->ifa_addr && address->ifa_addr->sa_family == AF_PACKET) {
            // In address->ifa_name the name of the current interface can be accessed.

            // Create a socket to communicate with the kernel asking for the IP address.
            int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
            struct ifreq interface_request{};
            strcpy(interface_request.ifr_name, address->ifa_name);
            ioctl(socket_fd, SIOCGIFHWADDR, &interface_request);
            close(socket_fd);

            std::array<uint8_t, 6> mac{};
            auto raw_mac = interface_request.ifr_hwaddr.sa_data;
            for (int i = 0; i != 6; ++i) {
                mac[i] = raw_mac[i];
            }

            if (mac == mac_address) {
                return true;
            }
        }
    }
#endif

    return false;
}

bool is_me(std::array<uint8_t, 16>& ipv6_address) {
#if defined(__linux__) || defined(__unix__)
    // Get all current network interfaces from the system.
    struct ifaddrs *addresses;
    getifaddrs(&addresses);

    for (struct ifaddrs *address = addresses; address != nullptr; address = address->ifa_next) {
        if (address->ifa_addr && address->ifa_addr->sa_family == AF_PACKET) {
            // In address->ifa_name the name of the current interface can be accessed.

            // Create a socket to communicate with the kernel asking for the IP address.
            int socket_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP);
            struct ifreq interface_request{};
            strcpy(interface_request.ifr_name, address->ifa_name);
            ioctl(socket_fd, SIOCGIFADDR, &interface_request);
            close(socket_fd);

            std::array<uint8_t, 16> ip{};
            auto raw_ip = ((sockaddr_in6*) &interface_request.ifr_addr)->sin6_addr;
            for (int i = 0; i != 16; ++i) {
                ip[i] = raw_ip.s6_addr[i];
            }

            if (ip == ipv6_address) {
                return true;
            }
        }
    }
#endif

    return false;
}