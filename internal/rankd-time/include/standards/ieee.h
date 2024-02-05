#ifndef RANKDTIME_IEEE_STANDARDS_H
#define RANKDTIME_IEEE_STANDARDS_H

namespace ieee802 {
    struct RationalGrouping {
        uint32_t numerator;
        uint32_t denominator;
    };

    struct PTPTimeGrouping {
        uint64_t seconds;
        uint32_t nanoseconds;
    };
}


#endif //RANKDTIME_IEEE_STANDARDS_H
