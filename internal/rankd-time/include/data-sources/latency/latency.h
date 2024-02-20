#ifndef RANKDTIME_LATENCY_H
#define RANKDTIME_LATENCY_H

#include <bitset>
#include <exception>
#include <map>
#include <ostream>
#include <sstream>

#ifndef LINUX_TC

#include <chrono>
#include <cstring>
#include <unistd.h>
#include <linux/netlink.h>
#include <linux/pkt_sched.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

#endif

#include "document.h"
#include "stringbuffer.h"
#include "writer.h"

#include "standards.h"
#include "utils/kernel.h"

#define OUTPUT_LATENCY_TYPE_CBS 1
#define OUTPUT_LATENCY_TYPE_TAS 2
#define OUTPUT_LATENCY_TYPE_CQF 4
#define OUTPUT_LATENCY_TYPE_ATS 8
#define OUTPUT_LATENCY_TYPE_ALL OUTPUT_LATENCY_TYPE_CBS || OUTPUT_LATENCY_TYPE_TAS || OUTPUT_LATENCY_TYPE_CQF || OUTPUT_LATENCY_TYPE_ATS

class Latency {
public:
    Latency();

    void snap();

    std::map<uint8_t, TAS::TAS *> tas();

    std::map<uint8_t, CBS::CBS *> cbs();

    void enable_json_output();

    void disable_json_output();

    void set_output(uint8_t type);

    ~Latency();

    [[nodiscard]] rapidjson::Document json() const;

    friend std::string print_latency_tas(const Latency& latency);
    friend std::string print_latency_cbs(const Latency& latency);
    friend std::ostream &operator<<(std::ostream &os, const Latency &latency);

private:
#ifndef LIUNX_TC

    void snap_tas_via_linux_tc();

    void snap_cbs_via_linux_tc();

    int _socket_descriptor = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    struct sockaddr_nl _local_point{};

#elif define(RELYUM)
    void snap_tas_via_relyum();
    void snap_cbs_via_relyum();
#endif
    std::map<uint8_t, TAS::TAS *> _time_aware_shaping_rules{};
    std::map<uint8_t, CBS::CBS *> _credit_based_shaping_rules{};
    bool _json_formatted_output = false;
    uint8_t _output_type;
};

#endif //RANKDTIME_LATENCY_H