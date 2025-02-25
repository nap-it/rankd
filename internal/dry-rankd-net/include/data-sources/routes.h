#ifndef DRYRANKD_NET_LIB_ROUTES_H
#define DRYRANKD_NET_LIB_ROUTES_H

#include <functional>
#include <iostream>
#include <string>
#include <set>

#include "utils/addresses.h"

struct NetworkRoute {
    int family;
    std::string destination;
    int destination_length;
    std::string source{};
    int source_length;
    std::string input_interface{};
    int tos;
    std::string output_interface{};
    int table_id;
    std::string gateway;
    std::string route_protocol;
    std::string scope;
    std::string type;
    std::string flags;
    int priority;
    int metric;
};

class NetworkRoutes {
public:
    NetworkRoutes(std::function<const std::vector<std::pair<uint8_t, uint8_t>>*()> topology, unsigned int own_address);
    void snap();
private:
    std::vector<NetworkRoute> _routes;
    std::function<const std::vector<std::pair<uint8_t, uint8_t>>*()> _topology;
    unsigned int _own_address;
};

#endif //DRYRANKD_NET_LIB_ROUTES_H
