#ifndef RANKD_TIME_LAT_CBS_H
#define RANKD_TIME_LAT_CBS_H

#include <cstdint>
#include <cstring>

#include <linux/rtnetlink.h>

class CBSEntry {
public:
    explicit CBSEntry(const struct rtattr* attribute);
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

private:
    CBSEntry _tas_entry;
};

#endif //RANKD_TIME_LAT_CBS_H
