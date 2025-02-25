#include "structs/identifier.h"

UUIDv4 generate_new_uuid() {
    UUIDv4 ulid = 0;
    ulid::EncodeTimeSystemClockNow(ulid);
    //ulid::EncodeEntropyRand(ulid);
    ulid::EncodeEntropy([]() { return 4; }, ulid);

    return ulid;
}

std::string display(const UUIDv4& uuid) {
    return ulid::Marshal(uuid);
}

bool is_same_randomness(const UUIDv4& left, const UUIDv4& right) {
    auto left_randomness = display(left).substr(10, 16);
    auto right_randomness = display(right).substr(10, 16);

    return left_randomness == right_randomness;
}

std::vector<uint8_t> marshall_into_vector(const UUIDv4& uuid) {
    return ulid::MarshalBinary(uuid);
}

UUIDv4 unmarshall_from_vector(const std::vector<uint8_t>& marshalled) {
    return ulid::UnmarshalBinary(marshalled);
}