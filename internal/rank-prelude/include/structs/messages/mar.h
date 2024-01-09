#ifndef RANK_PRELUDE_MAR_H
#define RANK_PRELUDE_MAR_H

#include <cstdint>
#include <vector>

#include "structs/message.h"
#include "structs/messages/header.h"
#include "structs/requesting_capabilities.h"

#define RANK_MAR_MESSAGE_LEN_LT_CODE_0 0
#define RANK_MAR_MESSAGE_LEN_LT_CODE_1 1
#define RANK_MAR_MESSAGE_LEN_LT_IP4 2
#define RANK_MAR_MESSAGE_LEN_LT_MAC 3
#define RANK_MAR_MESSAGE_LEN_LT_IP6 4
#define RANK_MAR_MESSAGE_LEN_LT_DDS 5
#define RANK_MAR_MESSAGE_LEN_LT_CODE_6 6
#define RANK_MAR_MESSAGE_LEN_LT_CODE_7 7

class MAR : public Message {
public:
    // Instance handlers.
    MAR(const UUIDv4& uuid, uint8_t priority, uint8_t listener_length, uint8_t* listener, uint16_t payload_length,
        uint8_t* payload) :
        Message(Header(MessageType::MAR, uuid)), _priority {priority}, _listener_length {listener_length}, _listener {listener},
        _payload_length {payload_length}, _payload {payload} {
    }
    MAR(const Header& header, uint8_t priority, uint8_t listener_length, uint8_t* listener, uint16_t payload_length,
        uint8_t* payload) :
        Message(header), _priority {priority}, _listener_length {listener_length}, _listener {listener},
        _payload_length {payload_length}, _payload {payload} {
    }
    MAR(const Header& header, const char* marshalled_data) : Message(header) {
    }

    // Marshalling features.
    RequestingCapabilities requirements() const;

    // Parsing tools.
    uint8_t expand_listener_length(uint8_t length) const;

    // Getters.
    uint8_t priority() const;
    uint8_t listener_length() const;
    uint8_t reserved() const;
    uint8_t* listener() const;
    uint16_t payload_length() const;
    uint8_t* payload() const;

    // Derived member methods.
    const char* raw_payload() const override;

    // Destructor.
    ~MAR();

private:
    uint8_t _priority;
    uint8_t _listener_length;
    uint8_t _reserved;
    uint8_t* _listener;
    uint16_t _payload_length;
    uint8_t* _payload;
};


#endif  // RANK_PRELUDE_MAR_H
