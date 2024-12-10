#ifndef RANK_PRELUDE_MARSHALLING_VALIDATION_H
#define RANK_PRELUDE_MARSHALLING_VALIDATION_H

#include <optional>

#include "utils/marshalling/validation/yang.h"

#include "libyang-cpp/Utils.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

bool validate_yang(const rapidjson::Document* json);

#endif // RANK_PRELUDE_MARSHALLING_VALIDATION_H