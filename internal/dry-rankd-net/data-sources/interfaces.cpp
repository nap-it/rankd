#include <utility>

#include "data-sources/interfaces.h"

NetworkDevices::NetworkDevices(std::function<const std::vector<int>*()> topology, unsigned int own_address) {
    _topology = std::move(topology);
    _own_address = own_address;
    _devices = std::map<int, NetworkDevice>();
    // TODO

    snap();
}

void NetworkDevices::snap() {
    // TODO
}
