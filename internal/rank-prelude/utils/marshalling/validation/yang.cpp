#include "utils/marshalling/validation/yang.h"

void clone_repository() {

}

void pull_repository() {

}

std::string get_yang_module_content(const RequestTypes& type) {
    switch (type) {
        case RequestTypes::CPURequest:
            return std::string(RANK_YANG_REPO_PATH) + "/" + RANK_YANG_SUFFIX + "/" + "nap-rank-comp-cpu-request.yang";
        case RequestTypes::MemoryRequest:
            return std::string(RANK_YANG_REPO_PATH) + "/" + RANK_YANG_SUFFIX + "/" + "nap-rank-comp-memory-request.yang";
        case RequestTypes::InterfaceRequest:
            return std::string(RANK_YANG_REPO_PATH) + "/" + RANK_YANG_SUFFIX + "/" + "nap-rank-net-interface-request.yang";
        case RequestTypes::Unknown:
            return "";
    }
}

std::string stringify(const RequestTypes& type) {
    switch (type) {
        case RequestTypes::CPURequest:
            return std::string(YANG_PREFIX) + ":" + "cpu-request";
        case RequestTypes::MemoryRequest:
            return std::string(YANG_PREFIX) + ":" + "memory-request";
        case RequestTypes::InterfaceRequest:
            return std::string(YANG_PREFIX) + ":" + "interface-request";
        case RequestTypes::Unknown:
            return "";
    }
}

RequestTypes typify(const std::string& string) {
    if (string == (std::string(YANG_PREFIX) + ":" + "cpu-request")) {
        return RequestTypes::CPURequest;
    }

    if (string == (std::string(YANG_PREFIX) + ":" + "memory-request")) {
        return RequestTypes::MemoryRequest;
    }

    if (string == (std::string(YANG_PREFIX) + ":" + "interface-request")) {
        return RequestTypes::InterfaceRequest;
    }

    return RequestTypes::Unknown;
}