#ifndef RANK_PRELUDE_REP_H
#define RANK_PRELUDE_REP_H

#include <array>

#include <cstdint>

#include "structs/message.h"
#include "structs/messages/header.h"

class REP : public Message {
public:
    // Instance handlers.
    REP(const UUIDv4& uuid, uint8_t listener_length, uint8_t* listener) :
        Message(Header(RANK_HEADER_VERSION, MessageType::REP, uuid)), _listener_length {listener_length}, _listener {listener} {
    }
    REP(const Header& header, uint8_t listener_length, uint8_t* listener) :
        Message(header), _listener_length {listener_length}, _listener {listener} {
    }
    REP(const Header& header, const std::vector<uint8_t>& marshalled_data) : Message(header) {
    }

    // Parsing tools.
    uint8_t expand_listener_length(uint8_t length) const;

    // Getters.
    uint8_t listener_length() const;
    uint8_t reserved() const;
    uint8_t* listener() const;

    // Derived member methods.
    const uint8_t* raw_payload() const override;

    // Destructor.
    ~REP();

private:
    uint8_t _listener_length;
    uint8_t _reserved;
    uint8_t* _listener;
};


#endif  // RANK_PRELUDE_REP_H
