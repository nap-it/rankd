#ifndef RANK_PRELUDE_CAPABILITYITEMTYPE_H
#define RANK_PRELUDE_CAPABILITYITEMTYPE_H

enum class CapabilityItemType {
    UNSPECIFIED = 0,

    // Time-related features.
    TIME_TAS = 1,
    TIME_CBS,

    // Network-related features.
    NET_BANDWIDTH = 128,
    NET_DDS,

    // Computation-related features.
    COMP_CPU = 256,
    COMP_MEMORY,
};

#endif  // RANK_PRELUDE_CAPABILITYITEMTYPE_H
