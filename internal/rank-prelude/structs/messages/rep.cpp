#include "structs/messages/rep.h"

uint8_t REP::expand_listener_length(uint8_t length) const {

}

uint8_t REP::listener_length() const {
    return _listener_length;
}

uint8_t REP::reserved() const {
    return _reserved;
}

std::vector<uint8_t> REP::listener() const {
    return _listener;
}

const std::vector<uint8_t> REP::raw_payload() const {
    std::vector<uint8_t> marshalled_data{};

    // Serialize header and add it to the marshalled data.
    auto marshalled_array = marshal_header(_header);
    marshalled_data.insert(marshalled_data.begin(), marshalled_array.begin(), marshalled_array.end());

    // Serialize first byte of message.
    uint8_t first_byte = ((_listener_length & 0x111) << 5) | (_reserved & 0x11111);
    marshalled_data.push_back(first_byte);

    // Copy listener ID to marshalled_data.
    switch (_listener_length) {
        case RANK_EAR_MESSAGE_LEN_LT_IP4:
            marshalled_data.insert(marshalled_data.end(), _listener.begin(), _listener.begin()+IPV4_ADDR_LEN);
            break;
        case RANK_EAR_MESSAGE_LEN_LT_MAC:
            marshalled_data.insert(marshalled_data.end(), _listener.begin(), _listener.begin()+MAC_ADDR_LEN);
            break;
        case RANK_EAR_MESSAGE_LEN_LT_IP6:
            marshalled_data.insert(marshalled_data.end(), _listener.begin(), _listener.begin()+IPV6_ADDR_LEN);
            break;
        case RANK_EAR_MESSAGE_LEN_LT_DDS:
            marshalled_data.insert(marshalled_data.end(), _listener.begin(), _listener.begin()+DDS_ADDR_LEN);
            break;
        default:
            return {};
    }

    return marshalled_data;

}

REP::~REP() {}
