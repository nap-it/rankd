#include "latency/linux/tc-helper.h"

TCNetlinkSocket::TCNetlinkSocket() {
    // Create the socket to connect with netlink, route family.
    _socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

    // Establish the local point for the connection.
    struct sockaddr_nl local_point{};
    local_point.nl_family = AF_NETLINK;
    local_point.nl_pad = 0;
    local_point.nl_pid = getpid();
    local_point.nl_groups = 0;

    // Bind the socket to the local point of connection to netlink.
    if (bind(_socket, (struct sockaddr *) &local_point, sizeof(local_point)) < 0) {
        // TODO Raise an exception in this case. An error could occur here in case of bad permissions.
    }

    // At this moment, route-netlink socket is considered ready-to-use.
    // We can now prepare messages and send requests.
}

const struct nlmsghdr *TCNetlinkSocket::request_get_for_qdiscs() {
    // Build the request message.
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

    // With the request done, just send it through the socket connection.
    if (send(_socket, &request, sizeof(request), 0) < 0) {
        // TODO Raise an exception as an error that occurred during the sending procedure.
    }

    // Receive the response from the requested data.
    char response[RESPONSE_MAX_LENGTH];
    ssize_t response_length = recv(_socket, response, sizeof(response), 0);
    if (response_length < 0) {
        // TODO Raise an exception as an error from the received content from the socket.
    }

    // Copy the response into a safely-allocated area in memory.
    struct nlmsghdr *response_copy = (struct nlmsghdr *) malloc(response_length);
    if (response_copy) {
        memcpy(response_copy, response, response_length);
    } else {
        // Handle memory allocation failure.
        // TODO Raise an exception or return an error code.
    }

    // Persist last response.
    _last_response = response_copy;
    _last_response_size = response_length;

    // Return the response as a netlink message.
    return response_copy;
}

const std::vector<qdisc> TCNetlinkSocket::qdiscs(std::function<bool(const struct rtattr *)> filter) {
    // Check if last response exists.
    if (_last_response == nullptr or _last_response_size == 0) {
        // TODO Raise an exception or return an error code.
    }

    // Initialize a vector to be returned.
    std::vector<struct tcmsg *> qdiscs_vector{};

    // Run over all queueing disciplines registered in the last response.
    while (NLMSG_OK(_last_response, _last_response_size)) {
        if (_last_response->nlmsg_type == RTM_NEWQDISC) {
            auto *content = (struct tcmsg *) NLMSG_DATA(_last_response);
            qdiscs_vector.push_back(content);
        }
    }

    // Filter results, if a filtering function is provided.
    if (filter != nullptr) {
        std::vector<struct tcmsg *> filtered_qdiscs_vector{};
        for (const auto &discipline: qdiscs_vector) {
            auto *attribute = (struct rtattr *) (discipline + 1);
            int attribute_length = _last_response->nlmsg_len - NLMSG_LENGTH(sizeof(*discipline));
            while (RTA_OK(attribute, attribute_length)) {
                if (filter(attribute)) {
                    filtered_qdiscs_vector.push_back(discipline);
                }
            }
        }

        return filtered_qdiscs_vector;
    } else {
        return qdiscs_vector;
    }
}