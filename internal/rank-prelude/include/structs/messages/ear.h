#ifndef RANK_PRELUDE_EAR_H
#define RANK_PRELUDE_EAR_H

#include <cstdint>
#include <vector>

#include "structs/message.h"
#include "structs/messages/header.h"
#include "structs/requesting_capabilities.h"

class EAR : public Message {
public:
    // Instance handlers.
    EAR(const Header& header, uint8_t priority, uint8_t listener_length, uint8_t* listener, uint16_t payload_length,
        uint8_t* payload) :
        Message(header), _priority {priority}, _listener_length {listener_length}, _listener {listener},
        _payload_length {payload_length}, _payload {payload} {
    }
    EAR(const Header& header, const char* marshalled_data) : Message(header) {
    }

    // Marshalling features.
    std::vector<RequestingCapabilities> unmarshal() const;

    // Parsing tools.
    uint8_t expand_listener_length(uint8_t length) const;

    // Getters.
    uint8_t priority() const;
    uint8_t listener_length() const;
    uint8_t reserved() const;
    uint8_t* listener() const;
    uint16_t payload_length() const;
    uint8_t* payload() const;

private:
    uint8_t _priority;
    uint8_t _listener_length;
    uint8_t _reserved;
    uint8_t* _listener;
    uint16_t _payload_length;
    uint8_t* _payload;
};


#endif  // RANK_PRELUDE_EAR_H
