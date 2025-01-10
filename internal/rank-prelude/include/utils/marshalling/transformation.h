#ifndef RANK_PRELUDE_MARSHALLING_TRANSFORMATION_H
#define RANK_PRELUDE_MARSHALLING_TRANSFORMATION_H

#include "rapidjson/document.h"

#include "structs/requesting_capabilities.h"
#include "utils/marshalling/validation.h"

rapidjson::Document transform_to_json(const RequestingCapabilities& capabilities);

RequestingCapabilities transform_to_requirements(const rapidjson::Document& json);

#endif // RANK_PRELUDE_MARSHALLING_TRANSFORMATION_H