#ifndef RANK_PRELUDE_IDENTIFIER_H
#define RANK_PRELUDE_IDENTIFIER_H

#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include "ulid.hh"

typedef ulid::ULID UUIDv4;

UUIDv4 generate_new_uuid();

UUIDv4 generate_new_uuid_with(time_t time);

std::string display(const UUIDv4& uuid);

bool is_same_timestamp(const UUIDv4& left, const UUIDv4& right);

std::vector<uint8_t> marshall_into_vector(const UUIDv4& uuid);

UUIDv4 unmarshall_from_vector(const std::vector<uint8_t>& marshalled);

#endif  // RANK_PRELUDE_IDENTIFIER_H
