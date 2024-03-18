#ifndef RANK_PRELUDE_MESSAGE_H
#define RANK_PRELUDE_MESSAGE_H

#include <array>
#include <cstdint>
#include <netinet/in.h>

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
    Header header() const;
    virtual const std::vector<uint8_t> raw_payload() const = 0;

    // Setters.
    void header(const Header& header);
    void header(const std::array<uint8_t, RANK_HEADER_LEN>& marshalled_data);

    // Destructor.
    virtual ~Message();
protected:
    Header _header;
private:
    size_t _length;
};


#endif  // RANK_PRELUDE_MESSAGE_H
