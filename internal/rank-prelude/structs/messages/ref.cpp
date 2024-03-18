#include "structs/messages/ref.h"

const std::vector<uint8_t> REF::raw_payload() const {
    std::vector<uint8_t> marshalled_data{};

    // Serialize header and add it to the marshalled data.
    auto marshalled_array = marshal_header(_header);
    std::copy(marshalled_array.begin(), marshalled_array.end(), marshalled_data.begin());

    return marshalled_data;
}

REF::~REF() {}