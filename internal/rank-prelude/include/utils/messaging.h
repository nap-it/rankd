#ifndef RANK_PRELUDE_MESSAGING_H
#define RANK_PRELUDE_MESSAGING_H

#include <cstdint>

#include "structs/messages/header.h"
#include "structs/requesting_capabilities.h"

uint16_t make_payload_length(const RequestingCapabilities& capabilities);

uint8_t* make_payload(const RequestingCapabilities& capabilities);

RequestingCapabilities parse_capabilities_from(const uint8_t* data);

Header new_prototype(const uint8_t* marshalled_data);

#endif  // RANK_PRELUDE_MESSAGING_H