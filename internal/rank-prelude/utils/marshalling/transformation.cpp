#include "utils/marshalling/transformation.h"

rapidjson::Document transform_to_json(const RequestingCapabilities& capabilities) {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value.
    rapidjson::Value value;

    // Create the main identifier for YANG-compliant Rank module.
    value.SetObject();
    json_document.AddMember("nap-rank:statement", value, allocator);

    // Create requirements array.
    value.SetArray();
    json_document["nap-rank:statement"].AddMember("requirements", value, allocator);

    // For each requirement in capabilities, transform it into the JSON value.
    for (const auto& [order, type_and_requirement] : capabilities.ordered_items()) {
        switch (type_and_requirement.first) {
            case CapabilityItemType::UNSPECIFIED:
                // TODO Handle this case.
                break;
            case CapabilityItemType::TIME_TAS:
                // TODO Handle this case.
                break;
            case CapabilityItemType::TIME_CBS:
                // TODO Handle this case.
                break;
            case CapabilityItemType::NET_BANDWIDTH:
                // TODO Handle this case.
                break;
            case CapabilityItemType::NET_DDS:
                // TODO Handle this case.
                break;
            case CapabilityItemType::COMP_CPU:
                // TODO Handle this case.
                break;
            case CapabilityItemType::COMP_MEMORY:
                // TODO Handle this case.
                break;
        }
    }

    return json_document;
}

RequestingCapabilities transform_to_requirements(const rapidjson::Document& json) {
    assert(json.HasMember("nap-rank:statement"));

    // Create an empty RequestingCapabilities structure to be returned later.
    RequestingCapabilities capabilities {};

    for (const auto& requirement : json["nap-rank:statement"]["requirements"].GetArray()) {
        auto type = static_cast<CapabilityItemType>((unsigned int) FnvHash(requirement["name"].GetString()));

        switch (type) {
            case CapabilityItemType::UNSPECIFIED:
                break;
            case CapabilityItemType::TIME_TAS:
                break;
            case CapabilityItemType::TIME_CBS:
                break;
            case CapabilityItemType::NET_BANDWIDTH:
                break;
            case CapabilityItemType::NET_DDS:
                break;
            case CapabilityItemType::COMP_CPU:
                break;
            case CapabilityItemType::COMP_MEMORY:
                break;
        }
    }

    return capabilities;
}
