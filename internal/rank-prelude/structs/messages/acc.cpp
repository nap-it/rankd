#include "structs/messages/acc.h"

const std::vector<uint8_t> ACC::raw_payload() const {
    std::vector<uint8_t> marshalled_data{};

    // Serialize header and add it to the marshalled data.
    auto marshalled_array = marshal_header(_header);
    marshalled_data.insert(marshalled_data.begin(), marshalled_array.begin(), marshalled_array.end());

    return marshalled_data;
}

ACC::~ACC() {}