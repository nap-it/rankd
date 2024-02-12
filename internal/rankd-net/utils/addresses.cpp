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