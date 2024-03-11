#ifndef RANK_PRELUDE_NEIGHBORING_H
#define RANK_PRELUDE_NEIGHBORING_H

#include <bitset>
#include <cassert>
#include <string>
#include <vector>

#include <unistd.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

#include "constants.h"

std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> get_connections_to(const std::vector<uint8_t> &target, bool is_dds = false);

unsigned int connections_cardinal(const std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> &connections);

#endif  // RANK_PRELUDE_NEIGHBORING_H
