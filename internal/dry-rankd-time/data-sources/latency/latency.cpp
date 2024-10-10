#include "data-sources/latency/latency.h"

Latency::Latency() {
    // TODO

    snap_tas();
    snap_cbs();
}

void Latency::snap() {
    // TODO
}

std::map<uint8_t, TAS::TAS *> Latency::tas() {
    return _time_aware_shaping_rules;
}

std::map<uint8_t, CBS::CBS *> Latency::cbs() {
    return _credit_based_shaping_rules;
}

void Latency::snap_tas() {
    // TODO
}

void Latency::snap_cbs() {
    // TODO
}

Latency::~Latency() {

}