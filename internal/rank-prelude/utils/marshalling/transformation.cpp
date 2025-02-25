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
    json_document.AddMember("nap-rank-requirements:requirements", value, allocator);

    // Create requirements array.
    value.SetArray();
    json_document["nap-rank-requirements:requirements"].AddMember("items", value, allocator);

    // For each requirement in capabilities, transform it into the JSON value.
    for (const auto& [order, type_and_requirement] : capabilities.ordered_items()) {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("order", order, allocator);

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
            case CapabilityItemType::COMP_CPU: {
                    rapidjson::Value sub_object(rapidjson::kObjectType);
                    sub_object.AddMember("cpu_cores", std::any_cast<uint>(type_and_requirement.second), allocator);
                    object.AddMember("requirement", sub_object, allocator);
                }
                break;
            case CapabilityItemType::COMP_MEMORY:
                // TODO Handle this case.
                break;
            default:
                // TODO Handle this case.
                break;
        }

        json_document["nap-rank-requirements:requirements"]["items"].PushBack(object, allocator);
    }

    return json_document;
}

RequestingCapabilities transform_to_requirements(const rapidjson::Document& json) {
    // Create an empty RequestingCapabilities structure to be returned later.
    RequestingCapabilities capabilities {};

    for (const auto& item : json["nap-rank-requirements:requirements"]["items"].GetArray()) {
        auto string = item["requirement"].MemberBegin()->name.GetString();
        auto type = static_cast<CapabilityItemType>((unsigned int) FnvHash(item["requirement"].MemberBegin()->name.GetString()));
        auto requirement = item["requirement"].GetObject();

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
            case CapabilityItemType::COMP_CPU: {
                    capabilities.add_item({CapabilityItemType::COMP_CPU, requirement["cpu_cores"].GetUint()}, item["order"].GetUint());
                }
                break;
            case CapabilityItemType::COMP_MEMORY:
                break;
        }
    }

    return capabilities;
}

std::string transform_to_string(const rapidjson::Document& json) {
    rapidjson::StringBuffer string_buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(string_buffer);
    json.Accept(writer);

    return {string_buffer.GetString(), string_buffer.GetSize()};
}
