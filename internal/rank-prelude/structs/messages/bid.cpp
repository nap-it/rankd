#include "structs/messages/bid.h"

float BID::value() const {
    return _value;
}

const uint8_t* BID::raw_payload() const {}

BID::~BID() {}