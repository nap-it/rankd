#include "data-sources/interfaces.h"

NetworkDevices::NetworkDevices() {
    _devices = std::map<int, NetworkDevice>();
    // TODO

    snap();
}

void NetworkDevices::snap() {
    // TODO
}
