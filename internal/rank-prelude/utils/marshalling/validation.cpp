#include "utils/marshalling/validation.h"

bool validate_yang(const rapidjson::Document* json) {
    assert(json != nullptr);

    // Retrieve the main Rank YANG module to confront this JSON against.
    libyang::Context context;
    context.
}