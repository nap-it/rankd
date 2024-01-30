#include "utils/messaging.h"

uint16_t make_payload_length(const RequestingCapabilities& capabilities) {

}

std::vector<uint8_t> make_payload(const RequestingCapabilities& capabilities) {

}

RequestingCapabilities parse_capabilities_from(const uint8_t* data) {

}

Header new_prototype(const uint8_t* marshalled_data) {

}

std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener_message_format(const std::vector<uint8_t>& connection) {
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener;

    std::copy_n(connection.begin(), connection.size(), listener.begin());

    return listener;
}