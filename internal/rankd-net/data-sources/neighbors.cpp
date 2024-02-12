#include "data-sources/neighbors.h"

NetworkNeighbors::NetworkNeighbors() {
    _neighbors = std::map<int, NetworkNeighbor>();

    snap();
}

void NetworkNeighbors::snap() {

}

rapidjson::Document NetworkNeighbors::json() const {

}

