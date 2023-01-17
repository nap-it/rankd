#include <iostream>
#include <optional>
#include <tuple>
#include <vector>

#include <unistd.h>

#include <asm/types.h>
#include <cstring>
#include <linux/netlink.h>
#include <linux/pkt_sched.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

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
    std::cerr << "Error" << std::endl;
    return -1;
  }

  return 0;
}

// From iproute2.
#define CLOCKID_INVALID (-1)
static const struct static_clockid {
  const char *name;
  clockid_t clockid;
} clockids_sysv[] = {{"REALTIME", CLOCK_REALTIME},
                     {"TAI", CLOCK_TAI},
                     {"BOOTTIME", CLOCK_BOOTTIME},
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

int main() {
  // Create the socket to connect with netlink, route family.
  int sock_id = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

  struct sockaddr_nl local_point {};
  local_point.nl_family = AF_NETLINK;
  local_point.nl_pad = 0;
  local_point.nl_pid = getpid();
  local_point.nl_groups = 0;

  if (bind(sock_id, (struct sockaddr *)&local_point, sizeof(local_point)) < 0) {
    std::cerr << "Cannot bind. Are you root?" << std::endl;
    return -1;
  }

  // At this moment, route-netlink socket is considered ready-to-use.
  // We can now prepare messages and send requests.

  struct {
    struct nlmsghdr header;
    struct tcmsg message;
  } request{};

  request.header.nlmsg_len = sizeof(request);
  request.header.nlmsg_type = RTM_GETQDISC;
  request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  request.header.nlmsg_seq = time(nullptr);
  request.header.nlmsg_pid = 0;
  request.message.tcm_family = AF_UNSPEC;

  // With the request done, we send it.
  if (send(sock_id, &request, sizeof(request), 0) < 0) {
    std::cerr << "Error while sending" << std::endl;
    return -1;
  }

  // Receive the response for the request.
  char response[8192];
  ssize_t response_length = recv(sock_id, response, sizeof(response), 0);
  if (response_length < 0) {
    std::cerr << "Error on receiving content from socket." << std::endl;
    return -1;
  }

  auto *response_header = (struct nlmsghdr *)response;
  while (NLMSG_OK(response_header, response_length)) {
    if (response_header->nlmsg_type == RTM_NEWQDISC) {
      auto *content = (struct tcmsg *)NLMSG_DATA(response_header);
      std::cout << "qdisc:" << std::endl;

      // Parse attributes in message.
      auto *attribute = (struct rtattr *)(content + 1);
      int attribute_length =
          response_header->nlmsg_len - NLMSG_LENGTH(sizeof(*content));
      while (RTA_OK(attribute, attribute_length)) {
        if (attribute->rta_type == TCA_KIND) {
          if (strcmp(static_cast<const char *>(RTA_DATA(attribute)),
                     "mqprio") == 0) {
            auto *qvalue = (struct tc_mqprio_qopt *)RTA_DATA(attribute);
            std::cout << "    - mqprio: "
                      << "num_tc " << qvalue->num_tc << " hw " << qvalue->hw
                      << std::endl;
          } else if (strcmp(static_cast<const char *>(RTA_DATA(attribute)),
                            "taprio") == 0) {
            attribute = RTA_NEXT(attribute, attribute_length);
            struct rtattr *taprio_attribute[TCA_TAPRIO_ATTR_MAX + 1];
            parse_rtattr_flags(
                taprio_attribute, TCA_TAPRIO_ATTR_MAX,
                static_cast<struct rtattr *>(RTA_DATA(attribute)),
                RTA_PAYLOAD(attribute), NLA_F_NESTED);
            if (taprio_attribute[TCA_TAPRIO_ATTR_PRIOMAP] != nullptr) {
              auto *qvalue = (struct tc_mqprio_qopt *)RTA_DATA(
                  taprio_attribute[TCA_TAPRIO_ATTR_PRIOMAP]);
              int num_tc = (int)qvalue->num_tc;
              std::vector<int> map{};
              for (int i = 0; i <= TC_PRIO_MAX; i++) {
                map.push_back((int)qvalue->prio_tc_map[i]);
              }
              std::vector<std::pair<int, int>> queues{};
              for (int i = 0; i != num_tc; i++) {
                queues.emplace_back((int)qvalue->count[i],
                                    (int)qvalue->offset[i]);
              }
              std::optional<__s32> clockid;
              if (taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CLOCKID]) {
                clockid = *(__s32 *)RTA_DATA(
                    taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CLOCKID]);
              }
              std::string clockid_string = clockid.has_value()
                                               ? get_clock_name(clockid.value())
                                               : "inexistent";
              std::optional<__u32> flags;
              if (taprio_attribute[TCA_TAPRIO_ATTR_FLAGS]) {
                flags =
                    *(__u32 *)RTA_DATA(taprio_attribute[TCA_TAPRIO_ATTR_FLAGS]);
              }
              std::optional<__u32> txtime_delay;
              if (taprio_attribute[TCA_TAPRIO_ATTR_TXTIME_DELAY]) {
                txtime_delay = *(__u32 *)RTA_DATA(
                    taprio_attribute[TCA_TAPRIO_ATTR_TXTIME_DELAY]);
              }

              // Get details on schedule.
              int64_t base_time = 0;
              int64_t cycle_time = 0;
              int64_t cycle_time_extension = 0;
              __s64 temporary_value;

              if (taprio_attribute[TCA_TAPRIO_ATTR_SCHED_BASE_TIME]) {
                memcpy(
                    &temporary_value,
                    RTA_DATA(taprio_attribute[TCA_TAPRIO_ATTR_SCHED_BASE_TIME]),
                    sizeof(temporary_value));
                base_time = temporary_value;
              }

              if (taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CYCLE_TIME]) {
                memcpy(&temporary_value,
                       RTA_DATA(
                           taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CYCLE_TIME]),
                       sizeof(temporary_value));
                cycle_time = temporary_value;
              }

              if (taprio_attribute
                      [TCA_TAPRIO_ATTR_SCHED_CYCLE_TIME_EXTENSION]) {
                memcpy(
                    &temporary_value,
                    RTA_DATA(taprio_attribute
                                 [TCA_TAPRIO_ATTR_SCHED_CYCLE_TIME_EXTENSION]),
                    sizeof(temporary_value));
                cycle_time_extension = temporary_value;
              }

              struct rtattr *schedule_pointer =
                  taprio_attribute[TCA_TAPRIO_ATTR_SCHED_ENTRY_LIST];
              std::vector<std::tuple<__u32, __u32, __u32, __u32>> schedule;
              struct rtattr *item;
              int remainder;
              if (schedule_pointer != nullptr) {
                remainder = RTA_PAYLOAD(schedule_pointer);
                for (item = (struct rtattr *)RTA_DATA(schedule_pointer);
                     RTA_OK(item, remainder);
                     item = RTA_NEXT(item, remainder)) {
                  struct rtattr
                      *schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_MAX + 1];
                  __u32 index = 0, command = 0, gatemask = 0, interval = 0;

                  parse_rtattr_flags(schedule_attribute,
                                     TCA_TAPRIO_SCHED_ENTRY_MAX,
                                     (struct rtattr *)(RTA_DATA(item)),
                                     RTA_PAYLOAD(item), NLA_F_NESTED);

                  if (schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_INDEX]) {
                    index = *(__u32 *)RTA_DATA(
                        schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_INDEX]);
                  }

                  if (schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_CMD]) {
                    command = *(__u32 *)RTA_DATA(
                        schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_CMD]);
                  }

                  if (schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_GATE_MASK]) {
                    gatemask = *(__u32 *)RTA_DATA(
                        schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_GATE_MASK]);
                  }

                  if (schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_INTERVAL]) {
                    interval = *(__u32 *)RTA_DATA(
                        schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_INTERVAL]);
                  }

                  schedule.emplace_back(index, command, gatemask, interval);
                }
              }
              std::cout << "    - taprio: "
                        << "num_tc " << num_tc << " map ";
              for (const auto &priority : map) {
                std::cout << priority << " ";
              }
              std::cout << "queues ";
              for (const auto &[c, o] : queues) {
                std::cout << c << "@" << o << " ";
              }
              std::cout << "base-time " << base_time << " ";
              for (const auto &[s_idx, s_cmd, s_gmk, s_int] : schedule) {
                std::cout << "sched-time cmd " << s_cmd << " index " << s_idx
                          << " gatemask " << s_gmk << " interval " << s_int
                          << " ";
              }
              std::cout << "flags " << (flags.has_value() ? flags.value() : 0)
                        << " ";
              std::cout << "txtime-delay "
                        << (txtime_delay.has_value() ? txtime_delay.value() : 0)
                        << " ";
              std::cout << "clockid " << clockid_string << std::endl;
            }
          } else if (strcmp(static_cast<const char *>(RTA_DATA(attribute)),
                            "pfifo_fast") == 0) {
            auto *qvalue = (struct tc_fifo_qopt *)RTA_DATA(attribute);
            std::cout << "    - pfifo_fast: "
                      << "limit " << qvalue->limit << std::endl;
          } else if (strcmp(static_cast<const char *>(RTA_DATA(attribute)),
                            "cbs") == 0) {
            auto *qvalue = (struct tc_cbs_qopt *)RTA_DATA(attribute);
            std::cout << "    - cbs: "
                      << "idleSlope " << qvalue->idleslope << " sendSlope "
                      << qvalue->sendslope << " hiCredit " << qvalue->hicredit
                      << " loCredit " << qvalue->locredit << " offload "
                      << qvalue->offload << std::endl;
          } else if (strcmp(static_cast<const char *>(RTA_DATA(attribute)),
                            "etf") == 0) {
            auto *qvalue = (struct tc_etf_qopt *)RTA_DATA(attribute);
            std::cout << "    - etf: "
                      << "clockid " << qvalue->clockid << " delta "
                      << qvalue->delta << " flags " << qvalue->flags
                      << std::endl;
          } else if (strcmp(static_cast<const char *>(RTA_DATA(attribute)),
                            "fq_codel") == 0) {
            auto *qvalue = (struct tc_fifo_qopt *)RTA_DATA(attribute);
            std::cout << "    - fifo: "
                      << "limit " << qvalue->limit << std::endl;
          }
        }
        attribute = RTA_NEXT(attribute, attribute_length);
      }
    }
    response_header = NLMSG_NEXT(response_header, response_length);
  }

  close(sock_id);

  return 0;
}