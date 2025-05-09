#include "utils/marshalling.h"

std::vector<uint8_t> marshall(const std::string& stringified_json) {
    // Parse stringified_json variable onto a JSON document.
    rapidjson::StringStream stream(stringified_json.c_str());
    rapidjson::Document json;
    json.ParseStream(stream);

    // Serialize as CBOR.
    auto serialized_object = serialize_json_as_vector(json);

    return serialized_object;
}

RequestingCapabilities unmarshal(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        return {};
    }

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
    std::vector<uint8_t> marshalled_uuid{};
    std::copy_n(data.begin()+1, RANK_UUID_LENGTH, std::back_inserter(marshalled_uuid));

    auto uuid = unmarshall_from_vector(marshalled_uuid);

    return Header(version, type, uuid);
}

std::array<uint8_t, RANK_HEADER_LEN> marshal_header(const Header& header) {
    std::array<uint8_t, RANK_HEADER_LEN> bytes;
    uint8_t first_byte = ((header.version() & RANK_HEADER_VERSION_BITMASK) << 6) | ((static_cast<unsigned int>(header.type()) & RANK_HEADER_TYPE_BITMASK) << 2);
    bytes[0] = first_byte;

    auto marshalled_uuid = marshall_into_vector(header.uuid());
    for (int byte = 0; byte != RANK_UUID_LENGTH; byte++) {
        bytes[1+byte] = marshalled_uuid.at(byte);
    }

    return bytes;
}