#ifndef RANK_PRELUDE_MARSHALLING_H
#define RANK_PRELUDE_MARSHALLING_H

class Header;

#include "structs/messages/header.h"

#include "utils/marshalling/serialization.h"
#include "utils/marshalling/transformation.h"
#include "utils/marshalling/validation.h"

//class Header;

uint8_t* marshal(const RequestingCapabilities& capabilities);

RequestingCapabilities unmarshal(const uint8_t* data);

Header unmarshal_header(const uint8_t* data);

uint8_t* marshal_header(const Header& header);

#endif  // RANK_PRELUDE_MARSHALLING_H
