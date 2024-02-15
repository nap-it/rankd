#ifndef RANKDTIME_LATENCY_H
#define RANKDTIME_LATENCY_H

#include <exception>
#include <map>
#include <ostream>

#ifndef LINUX_TC
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <linux/netlink.h>
#include <linux/pkt_sched.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#endif

#include "standards.h"
#include "utils/kernel.h"

class Latency {
public:
    Latency();
    void snap();
    std::map<uint8_t, TAS::TAS*> tas();
    std::map<uint8_t, CBS::CBS*> cbs();
    void enable_json_output();
    void disable_json_output();
    ~Latency();
private:
#ifndef LIUNX_TC
    void snap_tas_via_linux_tc();
    void snap_cbs_via_linux_tc();

    int _socket_descriptor = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    struct sockaddr_nl* _local_point = nullptr;

#elif define(RELYUM)
    void snap_tas_via_relyum();
    void snap_cbs_via_relyum();
#endif
    std::map<uint8_t, TAS::TAS*> _time_aware_shaping_rules{};
    std::map<uint8_t, CBS::CBS*> _credit_based_shaping_rules{};
    bool _json_formatted_output = false;
};

#endif //RANKDTIME_LATENCY_H