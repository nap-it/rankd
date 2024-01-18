#ifndef RANK_PRELUDE_MESSAGING_H
#define RANK_PRELUDE_MESSAGING_H

#include <cstdint>

#include "structs/requesting_capabilities.h"

uint16_t make_payload_length(const RequestingCapabilities& capabilities);

uint8_t* make_payload(const RequestingCapabilities& capabilities);

#endif  // RANK_PRELUDE_MESSAGING_H