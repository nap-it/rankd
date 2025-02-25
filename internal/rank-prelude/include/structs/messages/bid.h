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
    BID(const Header& header, const std::vector<uint8_t>& marshalled_data) : Message(header) {
        uint8_t value[sizeof(float)];
        for (int i = 0; i != sizeof(float); i++) {
            value[i] = marshalled_data.at(i);
        }
        _value = *reinterpret_cast<float*>(value);
    }

    // Getters.
    float value() const;

    // Derived member methods.
    const std::vector<uint8_t> raw_payload() const override;
    std::string display() override;

    // Destructor.
    ~BID();
private:
    float _value;
};


#endif  // RANK_PRELUDE_BID_H
