#ifndef RANK_PRELUDE_NEIGHBORING_H
#define RANK_PRELUDE_NEIGHBORING_H

#include <string>
#include <vector>

#include "constants.h"

std::vector<std::vector<uint8_t>> get_connections_to(const std::vector<uint8_t>& target);

unsigned int connections_cardinal(const std::vector<std::vector<uint8_t>>& connections);

#endif  // RANK_PRELUDE_NEIGHBORING_H
