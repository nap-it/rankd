#include "structs/messages/bid.h"

float BID::value() const {
    return _value;
}

const std::vector<uint8_t> BID::raw_payload() const {
    std::vector<uint8_t> marshalled_data{};

    // Serialize header and add it to the marshalled data.
    auto marshalled_array = marshal_header(_header);
    marshalled_data.insert(marshalled_data.begin(), marshalled_array.begin(), marshalled_array.end());

    // Serialize bid value.
    uint8_t const * pointer = reinterpret_cast<uint8_t const *>(&_value);
    for (std::size_t i = 0; i != sizeof(float); i++) {
        marshalled_data.push_back(pointer[i]);
    }

    return marshalled_data;
}

BID::~BID() {}