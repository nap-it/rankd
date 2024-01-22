#ifndef RANK_PRELUDE_MARSHALLING_H
#define RANK_PRELUDE_MARSHALLING_H

#include "utils/marshalling/serialization.h"
#include "utils/marshalling/transformation.h"
#include "utils/marshalling/validation.h"

uint8_t* marshal(const RequestingCapabilities& capabilities);

RequestingCapabilities unmarshal(const uint8_t* data);

#endif  // RANK_PRELUDE_MARSHALLING_H
