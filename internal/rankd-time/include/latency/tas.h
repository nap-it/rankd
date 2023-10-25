#ifndef RANKD_TIME_LAT_TAS_H
#define RANKD_TIME_LAT_TAS_H

#include <cstdint>
#include <cstring>

#include <linux/rtnetlink.h>

class TASEntry {
public:
    static bool check_identity(const struct rtattr* attribute) {
        return attribute != nullptr && strcmp(static_cast<const char*>(RTA_DATA(attribute)), "taprio") == 0;
    }

private:

};

class TAS {
public:

private:
    TASEntry _tas_entry;
};

#endif //RANKD_TIME_LAT_TAS_H
