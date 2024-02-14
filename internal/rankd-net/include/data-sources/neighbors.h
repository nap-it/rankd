#ifndef RANKD_NET_LIB_NEIGHBORS_H
#define RANKD_NET_LIB_NEIGHBORS_H

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

struct NetworkNeighbor {
    uint8_t family;
    std::string state;
    std::vector<std::string> flags{};
    std::string l2_address;
    std::string l3_address;
};

class NetworkNeighbors {
public:
    NetworkNeighbors();
    void snap();
    void enable_json_output();
    rapidjson::Document json() const;
    friend std::ostream& operator<<(std::ostream& os, const NetworkNeighbors& neighbors);
private:
    std::vector<NetworkNeighbor> _neighbors;
    bool _json_formatted_output = false;
};

#endif //RANKD_NET_LIB_NEIGHBORS_H
