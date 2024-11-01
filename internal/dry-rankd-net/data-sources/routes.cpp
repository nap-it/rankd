#include "data-sources/routes.h"

#include <utility>

NetworkRoutes::NetworkRoutes(std::function<const std::vector<int>*()> topology, unsigned int own_address) {
    _topology = std::move(topology);
    _own_address = own_address;
    _routes = std::vector<NetworkRoute>();
    // TODO

    snap();
}

void NetworkRoutes::snap() {
    // TODO
}
