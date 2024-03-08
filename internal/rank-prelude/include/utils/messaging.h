#ifndef RANK_PRELUDE_MESSAGING_H
#define RANK_PRELUDE_MESSAGING_H

#include <algorithm>
#include <cstdint>

#include "structs/message.h"
#include "structs/messages/all.h"
#include "structs/messages/header.h"
#include "structs/requesting_capabilities.h"

uint16_t make_payload_length(const RequestingCapabilities& capabilities);

std::vector<uint8_t> make_payload(const RequestingCapabilities& capabilities);

RequestingCapabilities parse_capabilities_from(const std::array<uint8_t, RANK_HEADER_LEN>& data);

Header new_prototype(const std::array<uint8_t, RANK_HEADER_LEN>& marshalled_data);

std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener_message_format(const std::vector<uint8_t>& connection);

Message* parse_message_from_bytes(std::vector<uint8_t> bytes, bool from_network = true);

#endif  // RANK_PRELUDE_MESSAGING_H