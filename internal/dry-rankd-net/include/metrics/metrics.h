#ifndef DRYRANKD_NET_LIB_METRICS_H
#define DRYRANKD_NET_LIB_METRICS_H

#include <cstdint>
#include <random>
#include <vector>

uint8_t number_of_hops_to(const std::vector<uint8_t>& target);

double round_trip_time_to(const std::vector<uint8_t>& target);

double packet_delay_variation_to(const std::vector<uint8_t>& target);

double packet_loss_to(const std::vector<uint8_t>& target);

#endif //DRYRANKD_NET_LIB_METRICS_H
