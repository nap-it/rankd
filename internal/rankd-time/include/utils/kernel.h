
#include <cstring>
#include <exception>
#include <linux/rtnetlink.h>

// Adapted from iproute2 git repo.
int parse_rtattr_flags(struct rtattr *tb[], int max, struct rtattr *rta,
                       int len, unsigned short flags) {
    unsigned short type;

    memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
    while (RTA_OK(rta, len)) {
        type = rta->rta_type & ~flags;
        if ((type <= max) && (!tb[type]))
            tb[type] = rta;
        rta = RTA_NEXT(rta, len);
    }

    if (len) {
        throw std::exception(); // TODO Handle this exception.
    }

    return 0;
}

// From iproute2.
#define CLOCKID_INVALID (-1)
static const struct static_clockid {
    const char *name;
    clockid_t clockid;
} clockids_sysv[] = {{"REALTIME",  CLOCK_REALTIME},
                     {"TAI",       CLOCK_TAI},
                     {"BOOTTIME",  CLOCK_BOOTTIME},
                     {"MONOTONIC", CLOCK_MONOTONIC},
                     {nullptr}};

// From iproute2.
static const char *get_clock_name(clockid_t clockid) {
    const struct static_clockid *c;

    for (c = clockids_sysv; c->name; c++) {
        if (clockid == c->clockid)
            return c->name;
    }

    return "invalid";
}