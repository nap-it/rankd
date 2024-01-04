#ifndef RANK_PRELUDE_HEADER_H
#define RANK_PRELUDE_HEADER_H

#include "structs/messages/message_type.h"

class Header {
public:
    // Instance handling.
    Header(const MessageType& type);
    Header(const char* marshalled_data);

    // Marshalling features.
    const char* marshal();

    // Getters.
    size_t size() const;
    uint8_t version() const;
    const MessageType& type() const;
    const UUIDv4& uuid() const;
private:
    uint8_t _version;
    MessageType _type;
    uint8_t _reserved_field;
    UUIDv4 _uuid;
};


#endif  // RANK_PRELUDE_HEADER_H
