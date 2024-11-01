#ifndef RANKD_NET_LIB_UTILS_ADDRESSES_H
#define RANKD_NET_LIB_UTILS_ADDRESSES_H

#include <string>
#include <cstring>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

std::string mac_to_string(unsigned char* mac);

std::string ip_to_string(int family, void* addr);

/* Adapted from iproute2 project */
int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len);

/* Adapted from iproute2. */
static inline int rtm_get_table(struct rtmsg *r, struct rtattr **tb) {
    __u32 table = r->rtm_table;

    if (tb[RTA_TABLE])
        table = *((__u32 *) RTA_DATA(tb[RTA_TABLE]));
    return table;
}

std::string get_interface_name(int index);

#endif // RANKD_NET_LIB_UTILS_ADDRESSES_H