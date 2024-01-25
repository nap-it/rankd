#ifndef RANK_PRELUDE_MARSHALLING_VALIDATION_H
#define RANK_PRELUDE_MARSHALLING_VALIDATION_H

#include "utils/marshalling/validation/yang.h"

#include "rapidjson/document.h"

bool validate_yang(const rapidjson::Document* json);

#endif // RANK_PRELUDE_MARSHALLING_VALIDATION_H