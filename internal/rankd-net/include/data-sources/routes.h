#ifndef RANKD_NET_LIB_ROUTES_H
#define RANKD_NET_LIB_ROUTES_H

#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

// RapidJSON inclusions.
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

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
    NetworkRoutes();
    void snap();
    rapidjson::Document json() const;
    friend std::ostream& operator<<(std::ostream& os, const NetworkRoutes& routes);
private:
    std::vector<NetworkRoute> _routes;
};

#endif //RANKD_NET_LIB_ROUTES_H
