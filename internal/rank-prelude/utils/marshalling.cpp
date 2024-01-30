#include "utils/marshalling.h"

uint8_t* marshal(const RequestingCapabilities& capabilities) {
    auto capabilities_as_json = transform_to_json(capabilities);

    auto serialized_object = serialize_json(capabilities_as_json);

    return serialized_object;
}

RequestingCapabilities unmarshal(const std::vector<uint8_t>& data) {
    auto capabilities_as_json = deserialize_json(data.data(), data.size());

    auto yang_compliant_json = validate_yang(&capabilities_as_json);
    if (not yang_compliant_json) {
        throw std::exception(); // TODO Would it be better to simply return another thing? Like a Rust's result?
    }

    auto capabilities = transform_to_requirements(capabilities_as_json);

    return capabilities;
}

Header unmarshal_header(const std::array<uint8_t, RANK_HEADER_LEN>& data) {
    uint8_t version = (data[0] >> 6) & RANK_HEADER_VERSION_BITMASK;
    MessageType type = static_cast<MessageType>((data[0] >> 2) & RANK_HEADER_TYPE_BITMASK);

    // Retrieve UUID from data byte stream.
    UUIDv4 uuid = 0;
    for (int byte = 0; byte != RANK_UUID_LENGTH; byte++) {
        uuid = uuid << 8 | data[RANK_HEADER_LEN-1-byte] & 0xFF;
    }

    return Header(version, type, uuid);
}

std::array<uint8_t, RANK_HEADER_LEN> marshal_header(const Header& header) {
    std::array<uint8_t, RANK_HEADER_LEN> bytes;
    uint8_t first_byte = ((header.version() & RANK_HEADER_VERSION_BITMASK) << 6) | ((static_cast<unsigned int>(header.type()) & RANK_HEADER_TYPE_BITMASK) << 2);
    bytes[0] = first_byte;

    auto uuid = header.uuid();
    for (int byte = 0; byte != RANK_UUID_LENGTH; byte++) {
        bytes[RANK_HEADER_LEN-1-byte] = uuid & 0xFF;
        uuid >>= 8;
    }

    return bytes;
}