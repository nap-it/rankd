#ifndef RANK_PRELUDE_CAPABILITYITEMTYPE_H
#define RANK_PRELUDE_CAPABILITYITEMTYPE_H

#include <functional>

#include "constants.h"

enum class CapabilityItemType : unsigned int {
    UNSPECIFIED = 0,

    // Time-related features.
    TIME_TAS = RANK_YANG_KEY_TIME_TAS,
    TIME_CBS = RANK_YANG_KEY_TIME_CBS,

    // Network-related features.
    NET_BANDWIDTH = RANK_YANG_KEY_NET_BANDWIDTH,
    NET_DDS = RANK_YANG_KEY_NET_DDS,

    // Computation-related features.
    COMP_CPU = RANK_YANG_KEY_COMP_CPUUSAGE,
    COMP_MEMORY = RANK_YANG_KEY_COMP_MEMORY,
};

#endif  // RANK_PRELUDE_CAPABILITYITEMTYPE_H
