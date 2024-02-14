#ifndef RANKD_NET_LIB_INTERFACES_H
#define RANKD_NET_LIB_INTERFACES_H

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

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

// Based on rtnl_link_stats structure from <if_link.h>.
typedef rtnl_link_stats DeviceStatistics;

struct NetworkDevice {
    int index;
    unsigned int flags;
    std::string l2_address;
    std::string l2_broadcast_address;
    std::string name;
    std::vector<std::pair<int, std::string>> addresses{};
    uint32_t mtu;
    int link_type;
    std::string qdisc;
    DeviceStatistics statistics;
};

class NetworkDevices {
public:
    NetworkDevices();
    void snap();
    void enable_json_output();
    rapidjson::Document json() const;
    friend std::ostream& operator<<(std::ostream& os, const NetworkDevices& devices);
private:
    std::map<int, NetworkDevice> _devices;
    bool _json_formatted_output = false;
};

#endif //RANKD_NET_LIB_INTERFACES_H
