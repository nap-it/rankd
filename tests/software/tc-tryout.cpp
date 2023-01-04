#include <iostream>

#include <linux/types.h>
#include "json_print.h"
#include "libnetlink.h"
#include "ll_map.h"
#include "tc/tc_common.h"
#include "tc/tc_util.h"
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

static int filter_ifindex;
static __u32 filter_parent;
static __u32 filter_handle;

int print_qdisc(struct nlmsghdr *header, void *arg) {
  FILE *file = (FILE *)arg;
  struct tcmsg *message = static_cast<struct tcmsg *>(NLMSG_DATA(header));
  int length = header->nlmsg_len;
  struct rtattr *tb[TCA_MAX + 1];
  struct qdisc_util *qdisc;
  char abuf[256];

  if (header->nlmsg_type != RTM_NEWQDISC &&
      header->nlmsg_type != RTM_DELQDISC) {
    std::cout << "Not a qdisc" << std::endl;
    return 0;
  }

  length -= NLMSG_LENGTH(sizeof(*message));
  if (length < 0) {
    std::cerr << "Wrong length" << std::endl;
    return -1;
  }

  if (filter_ifindex && filter_ifindex != message->tcm_ifindex) {
    return 0;
  }

  if (filter_handle && filter_handle != message->tcm_handle) {
    return 0;
  }

  if (filter_parent && filter_parent != message->tcm_parent) {
    return 0;
  }

  parse_rtattr_flags(tb, TCA_MAX, TCA_RTA(message), length, NLA_F_NESTED);

  if (tb[TCA_KIND] == nullptr) {
    std::cerr << "Null kind of qdisc" << std::endl;
    return -1;
  }

  open_json_object(nullptr);
  if (header->nlmsg_type == RTM_DELQDISC) {
    print_bool(PRINT_ANY, "deleted", "deleted ", true);
  }

  if (header->nlmsg_type == RTM_NEWQDISC &&
      (header->nlmsg_flags & NLM_F_CREATE) &&
      (header->nlmsg_flags & NLM_F_REPLACE)) {
    print_bool(PRINT_ANY, "replaced", "replaced ", true);
  }

  if (header->nlmsg_type == RTM_NEWQDISC &&
      (header->nlmsg_flags & NLM_F_CREATE) &&
      (header->nlmsg_flags & NLM_F_EXCL)) {
    print_bool(PRINT_ANY, "added", "added ", true);
  }

  print_string(PRINT_ANY, "kind", "qdisc %s", rta_getattr_str(tb[TCA_KIND]));
  sprintf(abuf, "%x:", message->tcm_handle >> 16);
  print_string(PRINT_ANY, "handle", " %s", abuf);
  print_string(PRINT_FP, nullptr, " ", nullptr);

  if (filter_ifindex == 0) {
    print_devname(PRINT_ANY, message->tcm_ifindex);
  }

  if (message->tcm_parent == TC_H_ROOT) {
    print_bool(PRINT_ANY, "root", "root ", true);
  } else if (message->tcm_parent) {
    print_tc_classid(abuf, sizeof(abuf), message->tcm_parent);
    print_string(PRINT_ANY, "parent", "parent %s", abuf);
  }

  if (message->tcm_info != 1) {
    print_uint(PRINT_ANY, "parent", "refcnt %u ", message->tcm_info);
  }

  if (tb[TCA_HW_OFFLOAD] && (rta_getattr_u8(tb[TCA_HW_OFFLOAD]))) {
    print_bool(PRINT_ANY, "offloaded", "offloaded ", true);
  }

  if (tb[TCA_INGRESS_BLOCK] &&
      RTA_PAYLOAD(tb[TCA_INGRESS_BLOCK]) >= sizeof(__u32)) {
    __u32 block = rta_getattr_u32(tb[TCA_INGRESS_BLOCK]);
    if (block) {
      print_uint(PRINT_ANY, "ingress_block", "ingress_block %u", block);
    }
  }

  if (tb[TCA_EGRESS_BLOCK] &&
      RTA_PAYLOAD(tb[TCA_EGRESS_BLOCK]) >= sizeof(__u32)) {
    __u32 block = rta_getattr_u32(tb[TCA_EGRESS_BLOCK]);
    if (block) {
      print_uint(PRINT_ANY, "egress_block", "egress_block %u ", block);
    }
  }

  if (strcmp("pfifo_fast", static_cast<char *>(RTA_DATA(tb[TCA_KIND]))) == 0) {
    qdisc = get_qdisc_kind("prio");
  } else {
    qdisc = get_qdisc_kind(static_cast<char *>(RTA_DATA(tb[TCA_KIND])));
  }

  open_json_object("options");
  if (tb[TCA_OPTIONS]) {
    if (qdisc) {
      qdisc->print_qopt(qdisc, file, tb[TCA_OPTIONS]);
    } else {
      std::cerr << "Cannot parse qdisc parameters" << std::endl;
    }
  }
  close_json_object();

  print_nl();

  if (show_details && tb[TCA_STAB]) {
    print_size_table(file, " ", tb[TCA_STAB]);
    print_nl();
  }

  if (show_stats) {
    struct rtattr *xstats = nullptr;

    if (tb[TCA_STATS] || tb[TCA_STATS2] || tb[TCA_XSTATS]) {
      print_tcstats_attr(file, tb, " ", &xstats);
      print_nl();
    }
    if (qdisc && xstats && qdisc->print_xstats) {
      qdisc->print_xstats(qdisc, file, xstats);
      print_nl();
    }
  }
  close_json_object();
  fflush(file);

  return 0;
}

int main() {

  // Initialize TC core functions.
  FILE *file;
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
  tick_in_usec = (double)t2us / us2t * clock_factor;

  // Open RTNL handler.
  struct rtnl_handle rth;
  if (rtnl_open(&rth, 0) < 0) {
    std::cerr << "Cannot procede to open rtnetlink" << std::endl;
    exit(1);
  }

  // Build the request.
  struct request request{};
  request.header.nlmsg_type = RTM_GETQDISC;
  request.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg));
  request.message.tcm_family = AF_UNSPEC;

  ll_init_map(&rth);
  if (rtnl_dump_request_n(&rth, &request.header) < 0) {
    std::cerr << "Cannot send request" << std::endl;
    return -3;
  }

  int json;
  new_json_obj(json);
  if (rtnl_dump_filter(&rth, print_qdisc, stdout) < 0) {
    std::cerr << "Cannot dump data" << std::endl;
    return -4;
  }

  return 0;
}