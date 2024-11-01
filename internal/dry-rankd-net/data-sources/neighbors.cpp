#include "data-sources/neighbors.h"

#include <utility>

NetworkNeighbors::NetworkNeighbors(std::function<const std::vector<int>*()> topology, unsigned int own_address) {
    _topology = std::move(topology);
    _own_address = own_address;
    _neighbors = std::vector<NetworkNeighbor>();
    // TODO

    snap();
}

void NetworkNeighbors::snap() {
    // TODO
}

std::vector <NetworkNeighbor> NetworkNeighbors::neighbors() {
    return _neighbors;
}
