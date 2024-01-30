#include "structs/messages/bid.h"

float BID::value() const {
    return _value;
}

const std::array<uint8_t, RANK_HEADER_LEN> BID::raw_payload() const {}

BID::~BID() {}