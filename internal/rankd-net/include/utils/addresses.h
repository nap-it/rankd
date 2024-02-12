#ifndef RANKD_NET_LIB_UTILS_ADDRESSES_H
#define RANKD_NET_LIB_UTILS_ADDRESSES_H

#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>

std::string mac_to_string(unsigned char* mac);

std::string ip_to_string(int family, void* addr);

#endif // RANKD_NET_LIB_UTILS_ADDRESSES_H