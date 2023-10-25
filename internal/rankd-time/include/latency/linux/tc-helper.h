#ifndef RANKD_TIME_LAT_LINUX_TCHELPER_H
#define RANKD_TIME_LAT_LINUX_TCHELPER_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <vector>

#include <unistd.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

#define RESPONSE_MAX_LENGTH 8192

typedef struct tcmsg *qdisc;

class TCNetlinkSocket {
public:
    TCNetlinkSocket();

    const struct nlmsghdr *request_get_for_qdiscs();

    std::vector<qdisc> qdiscs(std::function<bool(const struct rtattr *)> filter = nullptr);

private:
    struct nlmsghdr *_last_response = nullptr;
    ssize_t _last_response_size;
    int _socket;
};

#endif //RANKD_TIME_LAT_LINUX_TCHELPER_H
