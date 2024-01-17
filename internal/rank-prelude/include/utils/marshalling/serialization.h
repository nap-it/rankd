#ifndef RANK_PRELUDE_MARSHALLING_SERIALIZATION_H
#define RANK_PRELUDE_MARSHALLING_SERIALIZATION_H

#include <cstdint>

#include "rapidjson/document.h"

uint8_t* serialize_json(const rapidjson::Document& json);

rapidjson::Document deserialize_json(const uint8_t* data);

#endif // RANK_PRELUDE_MARSHALLING_SERIALIZATION_H