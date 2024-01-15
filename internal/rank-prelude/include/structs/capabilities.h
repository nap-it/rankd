#ifndef RANK_PRELUDE_CAPABILITIES_H
#define RANK_PRELUDE_CAPABILITIES_H

#include <any>
#include <map>
#include <optional>

#include "structs/capability_item_type.h"

class Capabilities {
public:
    // Marshalling features.
    static const char* marshal();
    static void unmarshal(const char* marshalled);

    // Summary producers.
    std::map<CapabilityItemType, std::any> produce_yang();

    // Getters.
    std::map<CapabilityItemType, std::any> items();

    // Operators.
    bool operator<(const Capabilities& prototype) const;
    bool operator==(const Capabilities& prototype) const;

private:
    std::map<CapabilityItemType, std::any> _items{};
};


#endif  // RANK_PRELUDE_CAPABILITIES_H
