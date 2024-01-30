#ifndef RANK_PRELUDE_ACC_H
#define RANK_PRELUDE_ACC_H

#include <array>

#include "constants.h"
#include "structs/message.h"
#include "structs/messages/header.h"

class ACC : public Message {
public:
    // Instance handlers.
    ACC(const UUIDv4& uuid) : Message(Header(RANK_HEADER_VERSION, MessageType::ACC, uuid)) {
    }
    ACC(const Header& header) : Message(header) {
    }
    ACC(const Header& header, const std::vector<uint8_t>& marshalled_data) : Message(header) {
    }

    // Derived member methods.
    const std::array<uint8_t, RANK_HEADER_LEN> raw_payload() const override;

    // Destructor.
    ~ACC();
};


#endif  // RANK_PRELUDE_ACC_H
