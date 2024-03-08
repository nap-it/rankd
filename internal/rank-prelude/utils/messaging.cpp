#include "utils/messaging.h"

uint16_t make_payload_length(const RequestingCapabilities& capabilities) {

}

std::vector<uint8_t> make_payload(const RequestingCapabilities& capabilities) {

}

RequestingCapabilities parse_capabilities_from(const std::array<uint8_t, RANK_HEADER_LEN>& data) {

}

Header new_prototype(const std::array<uint8_t, RANK_HEADER_LEN>& marshalled_data) {

}

std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener_message_format(const std::vector<uint8_t>& connection) {
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener;

    std::copy_n(connection.begin(), connection.size(), listener.begin());

    return listener;
}

Message* parse_message_from_bytes(std::vector<uint8_t> bytes, bool from_network) {
    // Check if the received message has length for a header, at least.
    if (bytes.size() < RANK_HEADER_LEN) {
        return nullptr;
    }

    // If bytes are from network, then switch to host.
    if (from_network) {
        for (auto& byte : bytes) {
            byte = ntohs(byte);
        }
    }

    // Retrieve header from bytes.
    std::array<uint8_t, RANK_HEADER_LEN> header_bytes{};
    std::copy_n(bytes.begin(), RANK_HEADER_LEN, header_bytes.begin());
    Header header = Header(header_bytes);

    // Parse the rest of the message, according to its type.
    std::vector<uint8_t> message_content{};
    std::copy_n(bytes.begin()+RANK_HEADER_LEN, bytes.size()-RANK_HEADER_LEN, message_content.begin());
    switch (header.type()) {
        case MessageType::NOTYPE:
            return nullptr;
        case MessageType::EAR:
            return new EAR(header, message_content);
        case MessageType::MAR:
            return new MAR(header, message_content);
        case MessageType::BID:
            return new BID(header, message_content);
        case MessageType::ACC:
            return new ACC(header);
        case MessageType::REF:
            return new REF(header);
        case MessageType::REP:
            return new REP(header, message_content);
    }
}