#ifndef RANK_PRELUDE_ACC_H
#define RANK_PRELUDE_ACC_H

#include "structs/message.h"
#include "structs/messages/header.h"

class ACC : public Message {
public:
    // Instance handlers.
    ACC(const UUIDv4& uuid) : Message(Header(MessageType::ACC, uuid)) {
    }
    ACC(const Header& header) : Message(header) {
    }
    ACC(const Header& header, const char* marshalled_data) : Message(header) {
    }

    // Derived member methods.
    const uint8_t* raw_payload() const override;

    // Destructor.
    ~ACC();
};


#endif  // RANK_PRELUDE_ACC_H
