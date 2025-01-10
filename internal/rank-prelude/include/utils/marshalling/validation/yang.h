#ifndef RANK_PRELUDE_MARSHALLING_VALIDATION_YANG_H
#define RANK_PRELUDE_MARSHALLING_VALIDATION_YANG_H

#include "libyang-cpp/Context.hpp"

#define YANG_PREFIX "nap-rank"

#define RANK_YANG_SUFFIX "private/nap/rank"
#define RANK_YANG_REQUIREMENTS "nap-rank-requirements.yang"

enum class RequestTypes {
    CPURequest,
    MemoryRequest,
    InterfaceRequest,
    Unknown
};

std::string stringify(const RequestTypes& type);

RequestTypes typify(const std::string& string);

std::string get_yang_module_content(const RequestTypes& type);

void clone_repository();

void pull_repository();



#endif  // RANK_PRELUDE_MARSHALLING_VALIDATION_YANG_H