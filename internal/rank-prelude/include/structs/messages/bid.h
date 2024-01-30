#ifndef RANK_PRELUDE_BID_H
#define RANK_PRELUDE_BID_H

#include <array>

#include "constants.h"
#include "structs/message.h"
#include "structs/messages/header.h"

class BID : public Message {
public:
    // Instance handlers.
    BID(const UUIDv4& uuid, float value) : Message(Header(RANK_HEADER_VERSION, MessageType::BID, uuid)), _value{value} {}
    BID(const Header& header, float value) : Message(header), _value{value} {}
    BID(const Header& header, const std::vector<uint8_t>& marshalled_data) : Message(header) {}

    // Getters.
    float value() const;

    // Derived member methods.
    const std::array<uint8_t, RANK_HEADER_LEN> raw_payload() const override;

    // Destructor.
    ~BID();
private:
    float _value;
};


#endif  // RANK_PRELUDE_BID_H
