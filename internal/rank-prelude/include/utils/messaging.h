#ifndef RANK_PRELUDE_MESSAGING_H
#define RANK_PRELUDE_MESSAGING_H

#include <algorithm>
#include <cstdint>

#include "structs/messages/header.h"
#include "structs/requesting_capabilities.h"

uint16_t make_payload_length(const RequestingCapabilities& capabilities);

std::vector<uint8_t> make_payload(const RequestingCapabilities& capabilities);

RequestingCapabilities parse_capabilities_from(const uint8_t* data);

Header new_prototype(const uint8_t* marshalled_data);

std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener_message_format(const std::vector<uint8_t>& connection);

#endif  // RANK_PRELUDE_MESSAGING_H