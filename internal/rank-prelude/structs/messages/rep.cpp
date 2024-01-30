#include "structs/messages/rep.h"

uint8_t REP::expand_listener_length(uint8_t length) const {

}

uint8_t REP::listener_length() const {
    return _listener_length;
}

uint8_t REP::reserved() const {
    return _reserved;
}

uint8_t* REP::listener() const {
    return _listener;
}

const std::array<uint8_t, RANK_HEADER_LEN> REP::raw_payload() const {}

REP::~REP() {}
