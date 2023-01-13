#include <iostream>

#include <unistd.h>

#include <asm/types.h>
#include <cstring>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/pkt_sched.h>
#include <sys/socket.h>

int main() {
  // Create the socket to connect with netlink, route family.
  int sock_id = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

  struct sockaddr_nl local_point{};
  local_point.nl_family = AF_NETLINK;
  local_point.nl_pad = 0;
  local_point.nl_pid = getpid();
  local_point.nl_groups = 0;

  if (bind(sock_id, (struct sockaddr*)&local_point, sizeof(local_point)) < 0) {
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

  auto* response_header = (struct nlmsghdr*)response;
  while (NLMSG_OK(response_header, response_length)) {
    if (response_header->nlmsg_type == RTM_NEWQDISC) {
      auto* content = (struct tcmsg*) NLMSG_DATA(response_header);
      std::cout << "qdisc ****" << std::endl;

      // Parse attributes in message.
      auto* attribute = (struct rtattr*) (content+1);
      int attribute_length = response_header->nlmsg_len - NLMSG_LENGTH(sizeof(*content));
      while (RTA_OK(attribute, attribute_length)) {
        if (attribute->rta_type == TCA_KIND) {
          if (strcmp(static_cast<const char*>(RTA_DATA(attribute)), "mqprio") == 0) {
            auto* qvalue = (struct tc_mqprio_qopt*) RTA_DATA(attribute);
            std::cout << "    - mqprio: " << "num_tc " << qvalue->num_tc << " hw " << qvalue->hw << std::endl;
          } else if (strcmp(static_cast<const char*>(RTA_DATA(attribute)), "pfifo_fast") == 0) {
            auto* qvalue = (struct tc_fifo_qopt*) RTA_DATA(attribute);
            std::cout << "    - pfifo_fast: " << "limit " << qvalue->limit << std::endl;
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