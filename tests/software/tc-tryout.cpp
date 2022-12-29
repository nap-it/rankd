#include <iostream>

#include "libnetlink.h"
#include "ll_map.h"
#include "utils.h"
#include <ifaddrs.h>
#include <linux/rtnetlink.h>

struct request {
  struct nlmsghdr header;
  struct tcmsg message;
  char buffer[256];
};

static double clock_factor = 1;
static double tick_in_usec = 1;



int main() {

  // Initialize TC core functions.
  FILE* file;
  __u32 clock_res, t2us, us2t;

  file = fopen("/proc/net/psched", "r");
  if (file == nullptr) {
    return -1;
  }
  if (fscanf(file, "%08x%08x%08x", &t2us, &us2t, &clock_res) != 3) {
    fclose(file);
    return -2;
  }
  fclose(file);

  /* compatibility hack: for old iproute binaries (ignoring
	 * the kernel clock resolution) the kernel advertises a
	 * tick multiplier of 1000 in case of nano-second resolution,
	 * which really is 1. */
  if (clock_res == 1000000000) {
    t2us = us2t;
  }

  clock_factor = (double)clock_res / TIME_UNITS_PER_SEC;
  tick_in_usec = (double)t2us/us2t * clock_factor;

  // Open RTNL handler.
  struct rtnl_handle rth;
  if (rtnl_open(&rth, 0) < 0) {
    std::cerr << "Cannot procede to open rtnetlink" << std::endl;
    exit(1);
  }

  // Build the request.
  struct request request = {
      .header.nlmsg_type = RTM_GETQDISC,
      .header.nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg)),
      .message.tcm_family = AF_UNSPEC,
  };

  ll_init_map(&rth);
  if (rtnl_dump_request_n(&rth, &request.header) < 0) {
    std::cerr << "Cannot send request" << std::endl;
    return -3;
  }

  int json;
  new_json_obj(json);
  if (rtnl_dump_filter(&rth, print_qdisc, stdout) < 0) {

  }

  return 0;
}