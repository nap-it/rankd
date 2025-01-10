#include "utils/marshalling/serialization.h"

std::vector<uint8_t> serialize_json_as_vector(rapidjson::Document& json) {
    cbor_item_t* cbor = load_cbor_data_to_encode(&json, cbor_stream_decode);

    unsigned char* buffer;
    size_t buffer_size;
    cbor_serialize_alloc(cbor, &buffer, &buffer_size);

    return std::vector<uint8_t>{buffer, buffer+buffer_size};
}

rapidjson::Document deserialize_json(const uint8_t* data, int length) {
    struct cbor_load_result result;
    cbor_item_t* item = cbor_load(data, length, &result);

    if (result.error.code != CBOR_ERR_NONE) {
        // TODO Handle this error.
    }

    rapidjson::Document json;
    rapidjson::Value composition = load_cbor_to_decode(item, json.GetAllocator());
    json.CopyFrom(composition, json.GetAllocator());

    return json;
}

cbor_item_t* load_cbor_data_to_encode(rapidjson::Document* source, cbor_load_callback_t callback) {
    static struct cbor_callbacks callbacks = {
        .uint32 = &cbor_builder_uint32_callback,
        .uint64 = &cbor_builder_uint64_callback,
        .negint64 = &cbor_builder_negint64_callback,
        .negint32 = &cbor_builder_negint32_callback,
        .string = &cbor_builder_string_callback,
        .array_start = &cbor_builder_array_start_callback,
        .map_start = &cbor_builder_map_start_callback,
        .float4 = &cbor_builder_float4_callback,
        .float8 = &cbor_builder_float8_callback,
        .null = &cbor_builder_null_callback,
        .boolean = &cbor_builder_boolean_callback,
    };

    struct _cbor_stack stack = _cbor_stack_init();

    struct _cbor_decoder_context context = (struct _cbor_decoder_context) {
        .stack = &stack,
    };

    callback(source, &callbacks, &context);

    return context.root;
}

void cbor_stream_decode(void *src, const struct cbor_callbacks* callbacks, void* context) {
    auto* source = (rapidjson::Document*)(src);

    if (source->IsBool() and source->GetBool() == false) {
        callbacks->boolean(context, false);
        return;
    }
    if (source->IsBool() and source->GetBool() == true) {
        callbacks->boolean(context, true);
        return;
    }
    if (source->IsNull()) {
        callbacks->null(context);
        return;
    }
    if (source->IsFloat()) {
        callbacks->float4(context, source->GetFloat());
        return;
    }
    if (source->IsDouble()) {
        callbacks->float8(context, source->GetDouble());
        return;
    }
    if (source->IsInt()) {
        callbacks->negint32(context, source->GetInt());
        return;
    }
    if (source->IsInt64()) {
        callbacks->negint64(context, source->GetInt64());
        return;
    }
    if (source->IsUint()) {
        callbacks->uint32(context, source->GetUint());
        return;
    }
    if (source->IsUint64()) {
        callbacks->uint64(context, source->GetUint64());
        return;
    }
    if (source->IsString()) {
        callbacks->string(context, (unsigned char*) source->GetString(), source->GetStringLength());
        return;
    }
    if (source->IsArray()) {
        callbacks->array_start(context, source->GetArray().Size());
        for (int i = 0; i != source->GetArray().Size(); i++) {
            rapidjson::Document copy;
            copy.CopyFrom(source->GetArray()[i], source->GetAllocator());
            cbor_stream_decode(&copy, callbacks, context);
        }
        return;
    }
    if (source->IsObject()) {
        callbacks->map_start(context, source->GetObject().MemberCount());
        for (auto iterator = source->MemberBegin(); iterator != source->MemberEnd(); iterator++) {
            callbacks->string(context, (unsigned char*) iterator->name.GetString(), iterator->name.GetStringLength());

            rapidjson::Document copy;
            copy.CopyFrom(iterator->value, source->GetAllocator()); //source[i], source->GetAllocator());
            cbor_stream_decode(&copy, callbacks, context);
        }
        return;
    }
}

rapidjson::Value load_cbor_to_decode(cbor_item_t* item, rapidjson::Document::AllocatorType& allocator) {
    switch (cbor_typeof(item)) {
        case CBOR_TYPE_UINT:
            switch (cbor_int_get_width(item)) {
                case CBOR_INT_8:
                    return rapidjson::Value(cbor_get_uint8(item));
                case CBOR_INT_16:
                    return rapidjson::Value(cbor_get_uint16(item));
                case CBOR_INT_32:
                    return rapidjson::Value(cbor_get_uint32(item));
                case CBOR_INT_64:
                    return rapidjson::Value(cbor_get_uint64(item));
            }
        case CBOR_TYPE_NEGINT:
            return rapidjson::Value(cbor_get_int(item));
        case CBOR_TYPE_BYTESTRING:
            return rapidjson::Value("Unimplemented feature (Byte string).");
        case CBOR_TYPE_STRING:
            if (cbor_string_is_definite(item)) {
                char* null_terminated_string = static_cast<char *>(malloc(cbor_string_length(item) + 1));
                memcpy(null_terminated_string, cbor_string_handle(item), cbor_string_length(item));
                null_terminated_string[cbor_string_length(item)] = 0;
                rapidjson::Value value(null_terminated_string, cbor_string_length(item));
                free(null_terminated_string);
            }
            return rapidjson::Value("Unimplemented feature (Chunked string).");
        case CBOR_TYPE_ARRAY: {
            rapidjson::Value array(rapidjson::kArrayType);
            for (size_t i = 0; i != cbor_array_size(item); i++) {
                array.PushBack(load_cbor_to_decode(cbor_array_get(item, i), allocator), allocator);
            }
            return array;
        }
        case CBOR_TYPE_MAP: {
            rapidjson::Value object(rapidjson::kObjectType);
            for (size_t i = 0; i != cbor_map_size(item); i++) {
                const auto& pair = cbor_map_handle(item);

                std::string key;
                if (cbor_isa_string(cbor_map_handle(item)[i].key) and cbor_string_is_definite(cbor_map_handle(item)[i].key)) {
                    key.assign(reinterpret_cast<const char *>(cbor_string_handle(pair->key)), cbor_string_length(pair->key));
                }

                rapidjson::Value key_value(key.c_str(), allocator);
                object.AddMember(key_value, load_cbor_to_decode(pair->value, allocator), allocator);
            }
            return object;
        }
        case CBOR_TYPE_TAG:
            return rapidjson::Value("Unimplemented feature (tag).");
        case CBOR_TYPE_FLOAT_CTRL:
            if (cbor_float_ctrl_is_ctrl(item)) {
                if (cbor_is_bool(item)) {
                    return rapidjson::Value(cbor_get_bool(item));
                }
                if (cbor_is_null(item)) {
                    return rapidjson::Value{};
                }
                return rapidjson::Value("Unimplemented feature (Control string).");
            }
            switch (cbor_float_get_width(item)) {
                case CBOR_FLOAT_0:
                    return rapidjson::Value(cbor_float_get_float(item));
                case CBOR_FLOAT_16:
                    return rapidjson::Value(cbor_float_get_float2(item));
                case CBOR_FLOAT_32:
                    return rapidjson::Value(cbor_float_get_float4(item));
                case CBOR_FLOAT_64:
                    return rapidjson::Value(cbor_float_get_float8(item));
            }
    }
    return rapidjson::Value{};
}