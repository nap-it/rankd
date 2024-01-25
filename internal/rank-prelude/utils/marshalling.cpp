#include "utils/marshalling.h"

uint8_t* marshal(const RequestingCapabilities& capabilities) {
    auto capabilities_as_json = transform_to_json(capabilities);

    auto serialized_object = serialize_json(capabilities_as_json);

    return serialized_object;
}

RequestingCapabilities unmarshal(const uint8_t* data) {
    auto capabilities_as_json = deserialize_json(data);

    auto yang_compliant_json = validate_yang(&capabilities_as_json);
    if (not yang_compliant_json) {
        throw std::exception(); // TODO Would it be better to simply return another thing? Like a Rust's result?
    }

    auto capabilities = transform_to_requirements(capabilities_as_json);

    return capabilities;
}

Header unmarshal_header(const uint8_t* data) {

}

uint8_t* marshal_header(const Header& header) {

}