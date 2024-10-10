#include "data-sources/neighbors.h"

NetworkNeighbors::NetworkNeighbors() {
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
