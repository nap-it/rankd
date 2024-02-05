#ifndef RANKD_TIME_LAT_CBS_H
#define RANKD_TIME_LAT_CBS_H

#include <cstdint>
#include <cstring>
#include <set>

#include <linux/rtnetlink.h>
#include <linux/pkt_sched.h>

// Just for testing purposes.
#define LINUX_TARGET

#ifdef LINUX_TARGET
#include "linux/tc-helper.h"
#endif

class CBSEntry {
public:
#ifdef LINUX_TARGET
    explicit CBSEntry(const struct rtattr* attribute);
#endif
    static bool check_identity(const struct rtattr* attribute) {
        return attribute != nullptr && strcmp(static_cast<const char*>(RTA_DATA(attribute)), "cbs") == 0;
    }
private:
    int32_t _hi_credit;
    int32_t _lo_credit;
    int32_t _idle_slope;
    int32_t _send_slope;
    bool _offload;
};

class CBS {
public:
    CBS();
private:
#ifdef LINUX_TARGET
    TCNetlinkSocket _netlink_socket{};
#endif
    std::set<CBSEntry> _entries;
};

#endif //RANKD_TIME_LAT_CBS_H
