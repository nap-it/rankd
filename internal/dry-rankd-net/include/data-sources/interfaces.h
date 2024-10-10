#ifndef DRYRANKD_NET_LIB_INTERFACES_H
#define DRYRANKD_NET_LIB_INTERFACES_H

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include "utils/addresses.h"

struct DeviceStatistics {
    __u32   rx_packets;
    __u32   tx_packets;
    __u32   rx_bytes;
    __u32   tx_bytes;
    __u32   rx_errors;
    __u32   tx_errors;
    __u32   rx_dropped;
    __u32   tx_dropped;
    __u32   multicast;
    __u32   collisions;
    __u32   rx_length_errors;
    __u32   rx_over_errors;
    __u32   rx_crc_errors;
    __u32   rx_frame_errors;
    __u32   rx_fifo_errors;
    __u32   rx_missed_errors;
    __u32   tx_aborted_errors;
    __u32   tx_carrier_errors;
    __u32   tx_fifo_errors;
    __u32   tx_heartbeat_errors;
    __u32   tx_window_errors;
    __u32   rx_compressed;
    __u32   tx_compressed;
    __u32   rx_nohandler;
};

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
private:
    std::map<int, NetworkDevice> _devices;
};

#endif //DRYRANKD_NET_LIB_INTERFACES_H
