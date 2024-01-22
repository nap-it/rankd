#ifndef RANK_PRELUDE_MESSAGE_H
#define RANK_PRELUDE_MESSAGE_H

#include <cstdint>

#include "structs/identifier.h"
#include "structs/messages/header.h"

class Message {
public:
    // Instance handling.
    Message(const MessageType& type);
    Message(const Header& header);

    // Getters.
    uint8_t version() const;
    MessageType type() const;
    UUIDv4 uuid() const;
    size_t size() const;
    virtual const uint8_t* raw_payload() const = 0;

    // Setters.
    void header(const Header& header);
    void header(const uint8_t* marshalled_data);

    // Destructor.
    virtual ~Message();
private:
    size_t _length;
    Header _header;
};


#endif  // RANK_PRELUDE_MESSAGE_H
