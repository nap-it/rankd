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
            marshalled_data.insert(marshalled_data.end(), _listener.begin(), _listener.begin() + IPV4_ADDR_LEN);
            break;
        case RANK_EAR_MESSAGE_LEN_LT_MAC:
            marshalled_data.insert(marshalled_data.end(), _listener.begin(), _listener.begin() + MAC_ADDR_LEN);
            break;
        case RANK_EAR_MESSAGE_LEN_LT_IP6:
            marshalled_data.insert(marshalled_data.end(), _listener.begin(), _listener.begin() + IPV6_ADDR_LEN);
            break;
        case RANK_EAR_MESSAGE_LEN_LT_DDS:
            marshalled_data.insert(marshalled_data.end(), _listener.begin(), _listener.begin() + DDS_ADDR_LEN);
            break;
        default:
            return {};
    }

    return marshalled_data;
}

std::string REP::display() {
    std::stringstream message;

    message << "{ VER: " << (int) static_cast<Message *>(this)->version() << ", TYPE: "
            << message_type_to_string(static_cast<Message *>(this)->type()) << ", RSV: " << 0 << ", UUID: "
            << ::display(static_cast<Message *>(this)->uuid()) << ", LEN_LT: "
            << (int) listener_length() << ", RSV: " << 0 << ", LISTENER_ID: ";

    switch (_listener_length) {
        case RANK_MAR_MESSAGE_LEN_LT_CODE_0:
            message << (int) _listener.at(0);
            break;
        case RANK_MAR_MESSAGE_LEN_LT_IP4:
            for (int i = 0; i != IPV4_ADDR_LEN; i++) {
                message << (int) _listener.at(i);
                if (i != IPV4_ADDR_LEN - 1) {
                    message << ".";
                }
            }
            break;
        case RANK_MAR_MESSAGE_LEN_LT_MAC:
            for (int i = 0; i != MAC_ADDR_LEN; i++) {
                message << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) _listener.at(i);
                if (i != MAC_ADDR_LEN - 1) {
                    message << ":";
                }
            }
            break;
        case RANK_MAR_MESSAGE_LEN_LT_IP6:
            for (int i = 0; i != IPV6_ADDR_LEN; i++) {
                message << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) _listener.at(i);
                if (i != IPV6_ADDR_LEN - 1 and i % 4 == 0) {
                    message << ":";
                }
            }
            break;
        case RANK_MAR_MESSAGE_LEN_LT_DDS:
            for (int i = 0; i != DDS_ADDR_LEN; i++) {
                message << std::setfill('0') << std::setw(2) << std::right << std::hex << (int) _listener.at(i);
            }
            break;
        default:
            return {};
    }

    message << " }";

    return message.str();
}

REP::~REP() {}
