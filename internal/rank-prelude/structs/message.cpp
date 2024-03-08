#include "structs/message.h"

Message::Message(const MessageType& type) {
    _length = 0;

    UUIDv4 uuid = 0; // TODO Allow UUID to be able to have a initialization value.
    _header = Header(RANK_HEADER_VERSION, type, uuid);
}

Message::Message(const Header& header) {
    _length = 0;
    _header = header;
}

uint8_t Message::version() const {
    return _header.version();
}

MessageType Message::type() const {
    return _header.type();
}

UUIDv4 Message::uuid() const {
    return _header.uuid();
}

size_t Message::size() const {
    return _header.size();
}

Header Message::header() const {
    return _header;
}

void Message::header(const Header& header) {
    _header = header;
}

void Message::header(const std::array<uint8_t, RANK_HEADER_LEN>& marshalled_data) {
    _header = Header(marshalled_data);
}

Message::~Message() {

}