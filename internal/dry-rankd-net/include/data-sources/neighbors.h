#ifndef DRYRANKD_NET_LIB_NEIGHBORS_H
#define DRYRANKD_NET_LIB_NEIGHBORS_H

#include <iostream>
#include <string>
#include <vector>

#include "utils/addresses.h"

struct NetworkNeighbor {
    uint8_t family;
    uint8_t interface_index;
    std::string state;
    std::vector<std::string> flags{};
    std::string l2_address;
    std::string l3_address;
};

class NetworkNeighbors {
public:
    NetworkNeighbors();
    void snap();
    std::vector<NetworkNeighbor> neighbors();
private:
    std::vector<NetworkNeighbor> _neighbors;
};

#endif //DRYRANKD_NET_LIB_NEIGHBORS_H
