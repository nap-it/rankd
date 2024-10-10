#ifndef DRYRANKDTIME_LATENCY_H
#define DRYRANKDTIME_LATENCY_H

#include <bitset>
#include <exception>
#include <map>
#include <ostream>
#include <sstream>

#include "standards.h"

#define OUTPUT_LATENCY_TYPE_CBS 1
#define OUTPUT_LATENCY_TYPE_TAS 2
#define OUTPUT_LATENCY_TYPE_CQF 4
#define OUTPUT_LATENCY_TYPE_ATS 8
#define OUTPUT_LATENCY_TYPE_ALL OUTPUT_LATENCY_TYPE_CBS || OUTPUT_LATENCY_TYPE_TAS || OUTPUT_LATENCY_TYPE_CQF || OUTPUT_LATENCY_TYPE_ATS

class Latency {
public:
    Latency();
    void snap();
    std::map<uint8_t, TAS::TAS *> tas();
    std::map<uint8_t, CBS::CBS *> cbs();
    ~Latency();
private:
#ifndef LIUNX_TC
    void snap_tas();
    void snap_cbs();
    std::map<uint8_t, TAS::TAS *> _time_aware_shaping_rules{};
    std::map<uint8_t, CBS::CBS *> _credit_based_shaping_rules{};
};

#endif //DRYRANKDTIME_LATENCY_H