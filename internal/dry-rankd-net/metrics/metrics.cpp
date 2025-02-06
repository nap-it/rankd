#include "metrics/metrics.h"

uint8_t number_of_hops_to(const std::vector<uint8_t>& target) {
    switch (target.size()) {
        case 4:     // Case if IPv4.
            return 1;
        case 16:    // Case if IPv6 or DDS.
            return 2;
        default:
            return 0;
    }
}

double round_trip_time_to(const std::vector<uint8_t>& target) {
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_real_distribution<> distribution(0.0, 250.0);

    return distribution(generator);
}

double packet_delay_variation_to(const std::vector<uint8_t>& target) {
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_real_distribution<> distribution(0.0, 25.0);

    return distribution(generator);
}

// Verificar aplicação de modelo de Gilbert-Elliott.
double packet_loss_to(const std::vector<uint8_t>& target) {
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_real_distribution<> distribution(0.0, 40.0);

    return distribution(generator);
}