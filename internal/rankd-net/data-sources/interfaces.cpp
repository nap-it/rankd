#include "data-sources/interfaces.h"

NetworkDevices::NetworkDevices() {
    _devices = std::map<std::string, NetworkDevice>();

    snap();
}

void NetworkDevices::snap() {
    struct sockaddr_nl binding_address;
    int netlink_socket = socket(AF_INET, SOCK_RAW, NETLINK_ROUTE);
    if (netlink_socket < 0) {
        throw std::exception();
    }
    memset(&binding_address, 0, sizeof(binding_address));
    binding_address.nl_family = AF_NETLINK;
    binding_address.nl_pid = getpid();

    // Bind netlink socket and address.
    if (bind(netlink_socket, (struct sockaddr*) &binding_address, sizeof(binding_address)) < 0) {
        throw std::exception();
    }

    // Create request message to netlink for interface dump request.
    struct {
        struct nlmsghdr header;
        struct ifinfomsg message;
    } request;
}