#include "data-sources/latency/latency.h"

Latency::Latency() {
#ifndef LINUX_TC
    _local_point = new struct sockaddr_nl{AF_NETLINK, 0, static_cast<__u32>(getpid()), 0};
    if (bind(_socket_descriptor, (struct sockaddr*) _local_point, sizeof(_local_point)) < 0) {
        throw std::exception(); // TODO Handle this error due to not being able to bind. Are you root?
    }

    // From this moment on, route-netlink socket is considered ready-to-use.
#endif
}

void Latency::snap() {
#ifndef LINUX_TC
    snap_tas_via_linux_tc();
    snap_cbs_via_linux_tc();
#elifdef RELYUM
    snap_tas_via_relyum();
    snap_cbs_via_relyum();
#endif
}

#ifndef LINUX_TC
void Latency::snap_tas_via_linux_tc() {

}

void Latency::snap_cbs_via_linux_tc() {
    // Declare structure for kernel request on TC.
    struct {
        struct nlmsghdr header;
        struct tcmsg message;
    } request;

    // Fill up fields for request.
    request.header.nlmsg_len = sizeof(request);
    request.header.nlmsg_type = RTM_GETQDISC;
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    request.header.nlmsg_seq = time(nullptr);
    request.header.nlmsg_pid = 0;
    request.message.tcm_family = AF_UNSPEC;

    // With the request done, we send it.
    if (send(_socket_descriptor, &request, sizeof(request), 0) < 0) {
        throw std::exception(); // TODO Handle this error while attempting to send request.
    }

    // Receive the response and handle it accordingly.
    char response[8192];
    ssize_t response_length = recv(_socket_descriptor, response, sizeof(response), 0);
    if (response_length < 0) {
        throw std::exception(); // TODO Handle this error.
    }

    _credit_based_shaping_rules.clear();

    auto* response_header = (struct nlmsghdr*)response;
    while (NLMSG_OK(response_header, response_length)) {
        if (response_header->nlmsg_type == RTM_NEWQDISC) {
            auto* content = (struct tcmsg*) NLMSG_DATA(response_header);
            auto* attribute = (struct rtattr*) (content + 1);
            int attribute_length = response_header->nlmsg_len - NLMSG_LENGTH(sizeof(*content));
            while (RTA_OK(attribute, attribute_length)) {
                if (attribute->rta_type == TCA_KIND) {
                    if (strcmp(static_cast<const char*>(RTA_DATA(attribute)), "cbs") == 0) {
                        // Get interface index and use it as a new index to our map.
                        auto interface_index = ((struct tcmsg*) (RTA_DATA(attribute)))->tcm_ifindex;

                        // If there is not a single record of such an interface, create one.
                        if (_credit_based_shaping_rules.count(interface_index) == 0) {
                            _credit_based_shaping_rules[interface_index] = new CBS::CBS();
                        }

                        auto traffic_class = ((struct tcmsg*) (RTA_DATA(attribute)))->tcm_parent;
                        auto idle_slope = ((struct tc_cbs_qopt*) (RTA_DATA(attribute)))->idleslope;
                        _credit_based_shaping_rules[interface_index]->cbsa_parameters->push_back(CBS::CBSAParametersTableEntry({static_cast<uint8_t>(traffic_class), 0, idle_slope}));
                    }
                }
                attribute = RTA_NEXT(attribute, attribute_length);
            }
        }
        response_header = NLMSG_NEXT(response_header, response_length);
    }
}
#endif

Latency::~Latency() {
#ifndef LINUX_TC
    close(_socket_descriptor);
#endif
}