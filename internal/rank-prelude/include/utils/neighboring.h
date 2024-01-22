#ifndef RANK_PRELUDE_NEIGHBORING_H
#define RANK_PRELUDE_NEIGHBORING_H

#include <string>
#include <vector>

std::vector<std::vector<uint8_t>> get_connections_to(const std::string& target);

unsigned int connections_cardinal(const std::vector<std::vector<uint8_t>>& connections);

#endif  // RANK_PRELUDE_NEIGHBORING_H
