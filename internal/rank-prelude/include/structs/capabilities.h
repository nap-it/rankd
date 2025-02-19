#ifndef RANK_PRELUDE_CAPABILITIES_H
#define RANK_PRELUDE_CAPABILITIES_H

#include <any>
#include <map>
#include <optional>

#include "structs/capability_item_type.h"

class Capabilities {
public:
    // Setters.
    void add_item(const std::pair<CapabilityItemType, std::any>& item);
    void remove_item(const std::pair<CapabilityItemType, std::any>& item);

    // Getters.
    std::map<CapabilityItemType, std::any> items() const;

    // Operators.
    bool operator<(const Capabilities& prototype) const;
    bool operator==(const Capabilities& prototype) const;

private:
    std::map<CapabilityItemType, std::any> _items{};
};


#endif  // RANK_PRELUDE_CAPABILITIES_H
