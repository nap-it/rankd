#include "utils/marshalling/serialization.h"

uint8_t* serialize_json(const rapidjson::Document& json) {
    rapidjson::StringBuffer string_buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(string_buffer);
    json.Accept(writer);

    std::string stringified_json(string_buffer.GetString(), string_buffer.GetSize());

    cbor::output_dynamic output;
    cbor::encoder encoder(output);
    encoder.write_bytes(reinterpret_cast<const unsigned char*>(stringified_json.c_str()), stringified_json.size());

    return output.data();
}

rapidjson::Document deserialize_json(const uint8_t* data, int length) {
    cbor::input input((void*) data, length);
    cbor::listener_debug listener;
    cbor::decoder decoder(input, listener);

    decoder.run();

    char buffer[length];
    input.get_bytes(buffer, length);

    rapidjson::Document json_document;
    json_document.Parse(buffer);

    return json_document;
}