#include "utils/marshalling/validation.h"

bool validate_yang(const rapidjson::Document* document) {
    assert(document != nullptr);

    auto json = document->GetObject();

    // Retrieve the admission request type from the JSON, if possible. If not, throw exception.
    if (json.MemberCount() != 1) {
        if (json.ObjectEmpty()) {
            throw std::invalid_argument("Admission request was received empty.");
        }
        if (json.MemberCount() > 1) {
            throw std::invalid_argument("Admission request was received with more than one argument.");
        }
    }

    if (json.MemberBegin()->name != "nap-rank-requirements:requirements") {
        throw std::invalid_argument("Received admission request is not a list of requirements.");
    }

    if (not json.MemberBegin()->value.HasMember("items")) {
        throw std::invalid_argument("Received admission request has requirements, but not in an items array structure.");
    }

    if (not json.MemberBegin()->value["items"].IsArray()) {
        throw std::invalid_argument("Received admission request has requirements, but not in an array structure.");
    }
    auto requirements_as_json_array = json.MemberBegin()->value["items"].GetArray();

    std::optional<libyang::Context> context{std::in_place, std::nullopt, libyang::ContextOptions::NoYangLibrary | libyang::ContextOptions::SetPrivParsed};
    int expected_order = 0;
    for (const auto& pair : requirements_as_json_array) {
        if (not pair.HasMember("order") or not pair.HasMember("requirement")) {
            throw std::invalid_argument("Received admission request badly-defined requirement, in expected order " + std::to_string(expected_order) + ".");
        }

        int order = pair.FindMember("order")->value.GetUint();
        if (order != expected_order++) {
            throw std::invalid_argument("Received admission request badly-defined order, in expected order + std::to_string(expected_order) + .");
        }

        auto requirement_as_json_object = pair.FindMember("requirement")->value.GetObject();
        RequestTypes request_type = typify(requirement_as_json_object.MemberBegin()->name.GetString());
        if (request_type == RequestTypes::Unknown) {
            throw std::invalid_argument("Received admission request badly-defined requirement, in expected order + std::to_string(expected_order) + .");
        }

        // Retrieve the main Rank YANG module to confront this JSON against.
        rapidjson::StringBuffer string_buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(string_buffer);
        document->Accept(writer);

        std::string json_string(string_buffer.GetString(), string_buffer.GetSize());

        std::filesystem::path yang_module = std::filesystem::path(RANK_YANG_REPO_PATH) / RANK_YANG_SUFFIX / RANK_YANG_REQUIREMENTS;

        context->setSearchDir(std::filesystem::path(RANK_YANG_REPO_PATH));
        auto module = context->loadModule("nap-rank-requirements", std::nullopt);

        auto data_node = context->parseData(json_string, libyang::DataFormat::JSON, libyang::ParseOptions::Strict);
        libyang::validateAll(data_node, libyang::ValidationOptions::Present);
    }

    return true;
}

RequestTypes parse_admission_request_type(const rapidjson::Document* json) {
    return typify(json->MemberBegin()->name.GetString());
}