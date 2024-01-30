#ifndef RANK_PRELUDE_EAR_H
#define RANK_PRELUDE_EAR_H

#include <array>
#include <cstdint>
#include <vector>

#include "structs/message.h"
#include "constants.h"
#include "structs/messages/header.h"
#include "structs/requesting_capabilities.h"

#include "utils/marshalling.h"
#include "utils/messaging.h"

class EAR : public Message {
public:
    // Instance handlers.
    EAR(const UUIDv4& uuid, uint8_t priority, uint8_t listener_length, const std::array<uint8_t, RANK_LISTENER_MAX_LEN>& listener, uint16_t payload_length,
        const std::vector<uint8_t>& payload) :
        Message(Header(RANK_HEADER_VERSION, MessageType::EAR, uuid)), _priority {priority}, _listener_length {listener_length}, _listener {listener},
        _payload_length {payload_length}, _payload {payload} {
    }
    EAR(const Header& header, uint8_t priority, uint8_t listener_length, const std::array<uint8_t, RANK_LISTENER_MAX_LEN>& listener, uint16_t payload_length,
        const std::vector<uint8_t>& payload) :
        Message(header), _priority {priority}, _listener_length {listener_length}, _listener {listener},
        _payload_length {payload_length}, _payload {payload} {
    }
    EAR(const Header& header, const std::vector<uint8_t>& marshalled_data) : Message(header) {
    }

    // Marshalling features.
    RequestingCapabilities requirements() const;

    // Parsing tools.
    uint8_t expand_listener_length(uint8_t length) const;

    // Getters.
    uint8_t priority() const;
    uint8_t listener_length() const;
    uint8_t reserved() const;
    std::vector<uint8_t> listener() const;
    uint16_t payload_length() const;
    std::vector<uint8_t> payload() const;

    // Derived member methods.
    const std::array<uint8_t, RANK_HEADER_LEN> raw_payload() const override;

    // Destructor.
    ~EAR();

private:
    uint8_t _priority;
    uint8_t _listener_length;
    uint8_t _reserved;
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> _listener;
    uint16_t _payload_length;
    std::vector<uint8_t> _payload;
};


#endif  // RANK_PRELUDE_EAR_H
