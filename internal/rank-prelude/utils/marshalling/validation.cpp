#include "utils/marshalling/validation.h"

bool validate_yang(const rapidjson::Document* document) {
    assert(document != nullptr);

    auto json = document->GetObject();

    // Retrieve the admission request type from the JSON, if possible. If not, throw exception.
    if (json.MemberCount() != 1) {
        if (json.ObjectEmpty()) {
            throw std::invalid_argument("Admission request was received while empty.");
        }
        if (json.MemberCount() > 1) {
            throw std::invalid_argument("Admission request was received with more than one argument.");
        }
    }
    RequestTypes request_type = typify(json.MemberBegin()->name.GetString());
    if (request_type == RequestTypes::Unknown) {
        throw std::invalid_argument("Admission request was received with an invalid or unknown argument."); // FIXME: The detection of type cannot be done here...
    }

    // Retrieve the main Rank YANG module to confront this JSON against.
    rapidjson::StringBuffer string_buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(string_buffer);
    document->Accept(writer);

    std::string json_string(string_buffer.GetString(), string_buffer.GetSize());

    // Load given YANG module and fix YANG string variable.
    std::string yang_module = std::string(RANK_YANG_REPO_PATH) + "/" + RANK_YANG_SUFFIX + "/" + RANK_YANG_REQUIREMENTS;

    // Create a libyang context and parse the corresponding YANG module.
    libyang::Context context;
    try {
        context.parseModule(yang_module, libyang::SchemaFormat::YANG);
    } catch (const libyang::ErrorWithCode& error) {
        throw std::invalid_argument(error.what());
    }

    // Parse the JSON string as data to verify against.
    auto data_node = context.parseData(json_string, libyang::DataFormat::JSON, std::nullopt, libyang::ValidationOptions::NoState);

    // Validate the data against the module.
    libyang::validateAll(data_node, libyang::ValidationOptions::NoState);

    return data_node.has_value();
}

RequestTypes parse_admission_request_type(const rapidjson::Document* json) {
    return typify(json->MemberBegin()->name.GetString());
}