#ifndef RANKD_NET_LIB_INTERFACES_H
#define RANKD_NET_LIB_INTERFACES_H

#define RAPIDJSON_HAS_STDSTRING 1

#include <map>
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

// Based on rtnl_link_stats structure from <if_link.h>.
struct DeviceStatistics {
    uint32_t rx_packets;
    uint32_t tx_packets;
    uint32_t rx_bytes;
    uint32_t tx_bytes;
    uint32_t rx_errors;
    uint32_t tx_errors;
    uint32_t rx_dropped;
    uint32_t tx_dropped;
    uint32_t multicast;
    uint32_t collisions;
    uint32_t rx_length_errors;
    uint32_t rx_over_errors;
    uint32_t rx_crc_errors;
    uint32_t rx_frame_errors;
    uint32_t rx_fifo_errors;
    uint32_t rx_missed_errors;
    uint32_t tx_aborted_errors;
    uint32_t tx_carrier_errors;
    uint32_t tx_fifo_errors;
    uint32_t tx_heartbeat_errors;
    uint32_t tx_window_errors;
    uint32_t rx_compressed;
    uint32_t tx_compressed;
};

struct NetworkDevice {
    std::string l2_address;
    std::string l2_broadcast_address;
    std::string name;
    uint32_t mtu;
    int8_t link_type;
    std::string qdisc;
    DeviceStatistics statistics;
};

class NetworkDevices {
public:
    NetworkDevices();
    void snap();
    rapidjson::Document json() const;
    friend std::ostream& operator<<(std::ostream& os, const NetworkDevices& devices);
private:
    std::map<std::string, NetworkDevice> _devices;
};

#endif //RANKD_NET_LIB_INTERFACES_H
