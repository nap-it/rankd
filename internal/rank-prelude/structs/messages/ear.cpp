#include "structs/messages/ear.h"

RequestingCapabilities EAR::requirements() const {
    return unmarshal(_payload);
}

uint8_t EAR::expand_listener_length(uint8_t length) const {
    return 0; // TODO
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

    if (_listener_length == RANK_EAR_MESSAGE_LEN_LT_CODE_0) {
        listener.push_back(_listener.at(0));
    } else {
        for (int i = 0; i != _listener_length; ++i) {
            listener.push_back(_listener.at(i));
        }
    }

    return listener;
}

uint16_t EAR::payload_length() const {
    return _payload_length;
}

std::vector<uint8_t> EAR::payload() const {
    return _payload;
}

const std::vector<uint8_t> EAR::raw_payload() const {
    std::vector<uint8_t> marshalled_data{};

    // Serialize header and add it to the marshalled data.
    auto marshalled_array = marshal_header(_header);
    marshalled_data.insert(marshalled_data.begin(), marshalled_array.begin(), marshalled_array.end());

    // Serialize first byte of message.
    uint8_t first_byte = (_priority << 5) | ((_listener_length & 0x111) << 2) | (_reserved & 0x11);
    marshalled_data.push_back(first_byte);

    // Copy listener ID to marshalled_data.
    switch (_listener_length) {
        case RANK_EAR_MESSAGE_LEN_LT_CODE_0:
            marshalled_data.insert(marshalled_data.end(), _listener.begin(), _listener.begin()+SIMUZILLA_ADDR_LEN);
            break;
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

    // Add payload length to marshalled_data.
    uint8_t const * pointer = reinterpret_cast<uint8_t const *>(&_payload_length);
    for (std::size_t i = 0; i != sizeof(uint16_t); i++) {
        marshalled_data.push_back(pointer[i]);
    }

    // Add payload to marshalled_data.
    marshalled_data.insert(marshalled_data.end(), _payload.begin(), _payload.end());

    return marshalled_data;
}

EAR::~EAR() {}