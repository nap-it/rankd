#include "utils/marshalling/validation.h"

bool validate_yang(const rapidjson::Document* json) {
    assert(json != nullptr);

    // Retrieve the main Rank YANG module to confront this JSON against.
    rapidjson::StringBuffer string_buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(string_buffer);
    json->Accept(writer);

    std::string json_string(string_buffer.GetString(), string_buffer.GetSize());

    // YANG string variable.
    std::string yang_module; // FIXME Add correct sources to this variable.

    // Create a libyang context and parse the corresponding YANG module.
    libyang::Context context;
    context.parseModule(yang_module, libyang::SchemaFormat::YANG);

    // Parse the JSON string as data to verify against.
    auto data_node = context.parseData(json_string, libyang::DataFormat::JSON, std::nullopt, libyang::ValidationOptions::NoState);

    // Validate the data against the module.
    libyang::validateAll(data_node, libyang::ValidationOptions::NoState);

    return data_node.has_value();
}