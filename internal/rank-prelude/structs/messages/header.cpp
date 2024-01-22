#include "structs/messages/header.h"

Header::Header(const MessageType& type, const UUIDv4& uuid) {
    _type = type;
    _uuid = uuid;
}

Header::Header(const uint8_t* marshalled_data) {
    Header header = unmarshal_header(marshalled_data);
    _version = header._version;
    _type = header._type;
    _reserved_field = header._reserved_field;
    _uuid = header._uuid
}

Header::Header(const Header& header) {
    _version = header._version;
    _type = header._type;
    _reserved_field = header._reserved_field;
    _uuid = header._uuid
}

Header::Header() {}

const uint8_t* Header::marshal() const {
    return marshal_header(*this);
}

size_t Header::size() const {
    return RANK_HEADER_LEN;
}

uint8_t Header::version() const {
    return _version;
}

const MessageType& Header::type() const {
    return _type;
}

const UUIDv4& Header::uuid() const {
    return _uuid;
}