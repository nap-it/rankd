#include "structs/identifier.h"

UUIDv4 generate_new_uuid() {
    UUIDv4 ulid = 0;
    ulid::EncodeTimeSystemClockNow(ulid);
    ulid::EncodeEntropyRand(ulid);
    //ulid::EncodeEntropy([]() { return 4; }, ulid);

    return ulid;
}

UUIDv4 generate_new_uuid_with(time_t time) {
    UUIDv4 ulid = 0;
    ulid::EncodeTime(time, ulid);
    ulid::EncodeEntropyRand(ulid);

    return ulid;
}

std::string display(const UUIDv4& uuid) {
    return ulid::Marshal(uuid);
}

bool is_same_timestamp(const UUIDv4& left, const UUIDv4& right) {
    auto left_timestamp = marshall_into_vector(left);
    auto right_timestamp = marshall_into_vector(right);

    return std::equal(left_timestamp.begin(), left_timestamp.begin()+6, right_timestamp.begin());
}

std::vector<uint8_t> marshall_into_vector(const UUIDv4& uuid) {
    return ulid::MarshalBinary(uuid);
}

UUIDv4 unmarshall_from_vector(const std::vector<uint8_t>& marshalled) {
    return ulid::UnmarshalBinary(marshalled);
}