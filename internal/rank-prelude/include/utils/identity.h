#ifndef RANK_PRELUDE_IDENTITY_H
#define RANK_PRELUDE_IDENTITY_H

#include <array>
#include <cstdint>
#include <cstring>

#if defined(__linux__) || defined(__unix__)
#include <ifaddrs.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#endif

bool is_me(std::array<uint8_t, 4>& ipv4_address);

bool is_me(std::array<uint8_t, 6>& mac_address);

bool is_me(std::array<uint8_t, 16>& ipv6_address);

#endif  // RANK_PRELUDE_IDENTITY_H