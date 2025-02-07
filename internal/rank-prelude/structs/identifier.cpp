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

std::vector<uint8_t> marshall_into_vector(const UUIDv4& uuid) {
    return ulid::MarshalBinary(uuid);
}

UUIDv4 unmarshall_from_vector(const std::vector<uint8_t>& marshalled) {
    return ulid::UnmarshalBinary(marshalled);
}