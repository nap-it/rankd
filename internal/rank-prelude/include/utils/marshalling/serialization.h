#ifndef RANK_PRELUDE_MARSHALLING_SERIALIZATION_H
#define RANK_PRELUDE_MARSHALLING_SERIALIZATION_H

#include <cfloat>
#include <cmath>
#include <cstdint>
#include <vector>
#include <sstream>

#include "cbor.h"
#include "cbor/internal/builder_callbacks.h"
#include "cbor/internal/loaders.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

typedef void (*cbor_load_callback_t)(void*, const struct cbor_callbacks*, void*);

std::vector<uint8_t> serialize_json_as_vector(rapidjson::Document& json);

rapidjson::Document deserialize_json(const uint8_t* data, int length);

cbor_item_t* load_cbor_data_to_encode(rapidjson::Document* source, cbor_load_callback_t callback);

void cbor_stream_decode(void *src, const struct cbor_callbacks* callbacks, void* context);

rapidjson::Value load_cbor_to_decode(cbor_item_t* item, rapidjson::Document::AllocatorType& allocator);

#endif // RANK_PRELUDE_MARSHALLING_SERIALIZATION_H