#ifndef RANKD_TIME_SYNC_PTP_PTP_H
#define RANKD_TIME_SYNC_PTP_PTP_H

#include <map>
#include <string>
#include <vector>

#include <linux/ethtool.h>
#include <linux/if.h>
#include <sys/ioctl.h>

#define IOCTL_ETHTOOL_REQ 0x8946      // Ethtool family of IOCTL requests constant identifier.

struct ethtool_command_context {
  const char* devname;
  int fd;
  struct ifreq ifr;
};

std::map<std::string, std::vector<std::string>> get_ptp_capabilities(const std::string& interface);

#endif // RANKD_TIME_SYNC_PTP_PTP_H
