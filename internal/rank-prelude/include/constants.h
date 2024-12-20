#ifndef RANK_PRELUDE_CONSTANTS_H
#define RANK_PRELUDE_CONSTANTS_H

#define SIMUZILLA_ADDR_LEN 1
#define IPV4_ADDR_LEN 4
#define MAC_ADDR_LEN 6
#define IPV6_ADDR_LEN 16
#define DDS_ADDR_LEN 16

#define RANK_PORT 7265
#define RANK_IP_PROTO 0x9B
#define RANK_ETHERTYPE 0x7265
#define RANK_INTERFACE "rank0" // TODO Change this value or allow this value to be configured.
#define RANK_INTERFACE_TYPE_INFO "dummy"

#define RANK_EAR_TO_EAR_TIMEOUT 0
#define RANK_MAR_TO_MAR_TIMEOUT 1
#define RANK_BID_TO_BID_TIMEOUT 2
#define RANK_BIDS_WAITING_TIMEOUT 3
#define RANK_REP_TO_REP_TIMEOUT 4

#define RANK_MAX_PRIORITY 8
#define RANK_CURRENT_RESOURCES_EVAL_THRESHOLD 2/3

#define RANK_HEADER_VERSION_BITMASK 0b11
#define RANK_HEADER_TYPE_BITMASK 0b1111
#define RANK_UUID_LENGTH 16

#define RANK_HEADER_VERSION 1
#define RANK_HEADER_LEN 17
#define RANK_HEADER_RESERVED_FIELD 0

#define RANK_LISTENER_MAX_LEN 16

#define RANK_EAR_MESSAGE_LEN_LT_CODE_0 0
#define RANK_EAR_MESSAGE_LEN_LT_CODE_1 1
#define RANK_EAR_MESSAGE_LEN_LT_IP4 2
#define RANK_EAR_MESSAGE_LEN_LT_MAC 3
#define RANK_EAR_MESSAGE_LEN_LT_IP6 4
#define RANK_EAR_MESSAGE_LEN_LT_DDS 5
#define RANK_EAR_MESSAGE_LEN_LT_CODE_6 6
#define RANK_EAR_MESSAGE_LEN_LT_CODE_7 7

#define RANK_MAR_MESSAGE_LEN_LT_CODE_0 RANK_EAR_MESSAGE_LEN_LT_CODE_0
#define RANK_MAR_MESSAGE_LEN_LT_CODE_1 RANK_EAR_MESSAGE_LEN_LT_CODE_1
#define RANK_MAR_MESSAGE_LEN_LT_IP4 RANK_EAR_MESSAGE_LEN_LT_IP4
#define RANK_MAR_MESSAGE_LEN_LT_MAC RANK_EAR_MESSAGE_LEN_LT_MAC
#define RANK_MAR_MESSAGE_LEN_LT_IP6 RANK_EAR_MESSAGE_LEN_LT_IP6
#define RANK_MAR_MESSAGE_LEN_LT_DDS RANK_EAR_MESSAGE_LEN_LT_DDS
#define RANK_MAR_MESSAGE_LEN_LT_CODE_6 RANK_EAR_MESSAGE_LEN_LT_CODE_6
#define RANK_MAR_MESSAGE_LEN_LT_CODE_7 RANK_EAR_MESSAGE_LEN_LT_CODE_7

enum class IdentifierType {
    IPv4,
    MAC,
    IPv6,
    DDS,
    Simulation
};

inline const char* identifier_to_string(const IdentifierType& type) {
    switch (type) {
        case IdentifierType::IPv4:
            return "IPv4";
        case IdentifierType::MAC:
            return "MAC";
        case IdentifierType::IPv6:
            return "IPv6";
        case IdentifierType::DDS:
            return "DDS";
        case IdentifierType::Simulation:
            return "Simulation";
    }

    return "";
}

#include "utils/hashing.h"

constexpr unsigned int RANK_YANG_KEY_TIME_TAS = FnvHash("time-tas");
constexpr unsigned int RANK_YANG_KEY_TIME_CBS = FnvHash("time-cbs");

constexpr unsigned int RANK_YANG_KEY_NET_BANDWIDTH = FnvHash("net-bandwidth");
constexpr unsigned int RANK_YANG_KEY_NET_DDS = FnvHash("net-dds");

constexpr unsigned int RANK_YANG_KEY_COMP_CPUUSAGE = FnvHash("comp-cpu-usage");
constexpr unsigned int RANK_YANG_KEY_COMP_MEMORY = FnvHash("comp-memory");

#endif  // RANK_PRELUDE_CONSTANTS_H
