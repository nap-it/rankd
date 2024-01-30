#ifndef RANK_PRELUDE_REF_H
#define RANK_PRELUDE_REF_H

#include <array>

#include "structs/message.h"
#include "structs/messages/header.h"

class REF : public Message {
public:
    // Instance handlers.
    REF(const UUIDv4& uuid) : Message(Header(RANK_HEADER_VERSION, MessageType::REF, uuid)) {
    }
    REF(const Header& header) : Message(header) {
    }
    REF(const Header& header, const std::vector<uint8_t>& marshalled_data) : Message(header) {
    }

    // Derived member methods.
    const uint8_t* raw_payload() const override;

    // Destructor.
    ~REF();
};


#endif  // RANK_PRELUDE_REF_H
