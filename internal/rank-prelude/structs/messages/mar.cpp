#include "structs/messages/mar.h"

RequestingCapabilities MAR::requirements() const {
    return unmarshal(_payload);
}

uint8_t MAR::expand_listener_length(uint8_t length) const {

}

uint8_t MAR::priority() const {
    return _priority;
}

uint8_t MAR::listener_length() const {
    return _listener_length;
}

uint8_t MAR::reserved() const {
    return _reserved;
}

uint8_t* MAR::listener() const {
    return _listener;
}

uint16_t MAR::payload_length() const {
    return _payload_length;
}

uint8_t* MAR::payload() const {
    return _payload;
}

const uint8_t* MAR::raw_payload() const {}

MAR:: ~MAR() {}