#include "structs/messages/ear.h"

RequestingCapabilities EAR::requirements() const {
    return unmarshal(_payload);
}

uint8_t EAR::expand_listener_length(uint8_t length) const {

}

uint8_t EAR::priority() const {
    return _priority;
}

uint8_t EAR::listener_length() const {
    return _listener_length;
}

uint8_t EAR::reserved() const {
    return _reserved;
}

std::vector<uint8_t> EAR::listener() const {
    std::vector<uint8_t> listener;

    for (int i = 0; i != _listener_length; ++i) {
        listener.push_back(_listener.at(i));
    }

    return listener;
}

uint16_t EAR::payload_length() const {
    return _payload_length;
}

std::vector<uint8_t> EAR::payload() const {
    return _payload;
}

const std::array<uint8_t, RANK_HEADER_LEN> EAR::raw_payload() const {
    return {}; // TODO Não estará isto misturado?
}

EAR::~EAR() {}