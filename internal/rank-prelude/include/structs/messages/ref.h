#ifndef RANK_PRELUDE_REF_H
#define RANK_PRELUDE_REF_H

#include "structs/message.h"
#include "structs/messages/header.h"

class REF : public Message {
public:
    // Instance handlers.
    REF(const UUIDv4& uuid) : Message(Header(MessageType::REF, uuid)) {
    }
    REF(const Header& header) : Message(header) {
    }
    REF(const Header& header, const char* marshalled_data) : Message(header) {
    }

    // Derived member methods.
    const uint8_t* raw_payload() const override;

    // Destructor.
    ~REF();
};


#endif  // RANK_PRELUDE_REF_H
