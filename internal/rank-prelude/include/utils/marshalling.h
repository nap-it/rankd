#ifndef RANK_PRELUDE_MARSHALLING_H
#define RANK_PRELUDE_MARSHALLING_H

class Header;

#include <array>
#include <vector>

#include "constants.h"
#include "structs/messages/header.h"

#include "utils/marshalling/serialization.h"
#include "utils/marshalling/transformation.h"
#include "utils/marshalling/validation.h"

//class Header;

uint8_t* marshal(const RequestingCapabilities& capabilities);

RequestingCapabilities unmarshal(const std::vector<uint8_t>& data);

Header unmarshal_header(const std::array<uint8_t, RANK_HEADER_LEN>& data);

std::array<uint8_t, RANK_HEADER_LEN> marshal_header(const Header& header);

#endif  // RANK_PRELUDE_MARSHALLING_H
