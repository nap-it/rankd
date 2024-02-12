#ifndef RANKD_NET_LIB_NEIGHBORS_H
#define RANKD_NET_LIB_NEIGHBORS_H

#include <map>
#include <iostream>
#include <string>

#include <unistd.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

// RapidJSON inclusions.
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

struct NetworkNeighbor {
    uint8_t family;
    std::string state;
    std::string flags;
    std::string type;
};

class NetworkNeighbors {
public:
    NetworkNeighbors();
    void snap();
    rapidjson::Document json() const;
    friend std::ostream& operator<<(std::ostream& os, const NetworkNeighbors& neighbors);
private:
    std::map<int, NetworkNeighbor> _neighbors;
};

#endif //RANKD_NET_LIB_NEIGHBORS_H
