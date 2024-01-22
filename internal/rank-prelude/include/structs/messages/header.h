#ifndef RANK_PRELUDE_HEADER_H
#define RANK_PRELUDE_HEADER_H

#include <cstdint>

#include "structs/identifier.h"
#include "structs/messages/constants.h"
#include "structs/messages/message_type.h"

#include "utils/marshalling.h"

class Header {
public:
    // Instance handling.
    Header(const MessageType& type, const UUIDv4& uuid);
    Header(const uint8_t* marshalled_data);
    Header(const Header& header);
    Header();

    // Marshalling features.
    const uint8_t* marshal() const;

    // Getters.
    size_t size() const;
    uint8_t version() const;
    const MessageType& type() const;
    const UUIDv4& uuid() const;
private:
    uint8_t _version = RANK_HEADER_VERSION;
    MessageType _type = MessageType::NOTYPE;
    uint8_t _reserved_field = RANK_HEADER_RESERVED_FIELD;
    UUIDv4 _uuid{};
};


#endif  // RANK_PRELUDE_HEADER_H
