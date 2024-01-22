#include "structs/message.h"

Message::Message(const MessageType& type) {
    _length = 0;

    UUIDv4 uuid = 0; // TODO Allow UUID to be able to have a initialization value.
    _header = Header(type, uuid);
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

void Message::header(const Header& header) {
    _header = header;
}

void Message::header(const char* marshalled_data) {
    _header = Header(marshalled_data);
}

Message::~Message() {

}