#include "utils/neighboring.h"

std::vector<std::vector<uint8_t>> get_connections_to(const std::vector<uint8_t>& target) {
    // Get the type of target address.
    switch (target.size()) {
        case 4:
            // If the address is IPv4, check for routes in IPv4.
            break;
        case 6:
            // If the address is MAC, check for forwarding paths in Ethernet.
            break;
        case 16:
            // If the address is IPv6, check for routes in IPv6.
            break;
    }

    return {};
}

unsigned int connections_cardinal(const std::vector<std::vector<uint8_t>>& connections) {
    return connections.size();
}
