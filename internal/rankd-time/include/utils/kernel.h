#ifndef RANKDTIME_UTILS_H
#define RANKDTIME_UTILS_H

#include <cstring>
#include <exception>
#include <linux/rtnetlink.h>
#include <ctime>

// Adapted from iproute2 git repo.
int parse_rtattr_flags(struct rtattr *tb[], int max, struct rtattr *rta,
                       int len, unsigned short flags);

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

#endif // RANKDTIME_UTILS_H