#ifndef RANK_PRELUDE_CONSTANTS_H
#define RANK_PRELUDE_CONSTANTS_H

#define RANK_PORT 7265

#define RANK_EAR_TO_EAR_TIMEOUT 0
#define RANK_MAR_TO_MAR_TIMEOUT 1
#define RANK_BID_TO_BID_TIMEOUT 2
#define RANK_BIDS_WAITING_TIMEOUT 3
#define RANK_REP_TO_REP_TIMEOUT 4

#define RANK_MAX_PRIORITY 8
#define RANK_CURRENT_RESOURCES_EVAL_THRESHOLD 2/3

#include "utils/hashing.h"

constexpr unsigned int RANK_YANG_KEY_TIME_TAS = FnvHash("time-tas");
constexpr unsigned int RANK_YANG_KEY_TIME_CBS = FnvHash("time-cbs");

constexpr unsigned int RANK_YANG_KEY_NET_BANDWIDTH = FnvHash("net-bandwidth");
constexpr unsigned int RANK_YANG_KEY_NET_DDS = FnvHash("net-dds");

constexpr unsigned int RANK_YANG_KEY_COMP_CPUUSAGE = FnvHash("comp-cpu-usage");
constexpr unsigned int RANK_YANG_KEY_COMP_MEMORY = FnvHash("comp-memory");

#endif  // RANK_PRELUDE_CONSTANTS_H
