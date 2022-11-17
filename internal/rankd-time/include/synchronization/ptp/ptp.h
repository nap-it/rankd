#ifndef RANKD_TIME_SYNC_PTP_PTP_H
#define RANKD_TIME_SYNC_PTP_PTP_H

#include <cstring>
#include <map>
#include <string>
#include <vector>

#include <linux/ethtool.h>
#include <linux/if.h>
#include <sys/ioctl.h>

#ifndef RELEASE_TARGET
#include <iostream>
#include <ifaddrs.h>
#endif

#define IOCTL_ETHTOOL_REQ                                                      \
  0x8946 // Ethtool family of IOCTL requests constant identifier.
#define HW_CAPABILITY_MASK                                                     \
  0b1111101 // Mask to get if device has PTP hardware capabilities.
#define SW_CAPABILITY_MASK                                                     \
  0b0101100 // Mask to get if device has PTP software capabilities.
#define HW_TX_TS_MODE_MASK                                                     \
  0b0000110 // Mask to get if device has needed HW TS TX modes.
#define HW_RX_FILTER_MASK 0b1100000000000000 // Mask for RX filters.

struct ethtool_command_context {
  const char *devname;
  int fd;
  struct ifreq ifr;
};

enum class PTPCapability { UNCAPABLE, SOFTWARE_ONLY, HARDWARE };

std::map<std::string, std::vector<std::string>>
get_ptp_capabilities(const std::string &interface);
PTPCapability capability_level_in(
    const std::map<std::string, std::vector<std::string>> &information);
PTPCapability ptp_capability_type(const std::string &interface_name);

// Debug functions.
#ifndef RELEASE_TARGET
void debug_all_interfaces_ptp_capabilities();
#endif

#endif // RANKD_TIME_SYNC_PTP_PTP_H
