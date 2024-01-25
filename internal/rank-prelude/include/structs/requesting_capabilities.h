#ifndef RANK_PRELUDE_REQUESTINGCAPABILITIES_H
#define RANK_PRELUDE_REQUESTINGCAPABILITIES_H

#include <cstdint>

#include "structs/capabilities.h"

class RequestingCapabilities : public Capabilities {
public:
    // Constructors.
    RequestingCapabilities() : Capabilities() {
    }

    // Item's management.
    void add_item(const std::pair<CapabilityItemType, std::any&>& item, uint8_t place);
    void downgrade_item(uint8_t place, uint8_t to_place = UINT8_MAX);
    void upgrade_item(uint8_t place, uint8_t to_place = 0);
    const std::pair<CapabilityItemType, std::any&>& get_item(uint8_t place);
    void remove_item(uint8_t place);

    // Getters.
    std::map<uint8_t, std::pair<CapabilityItemType, std::any&>> ordered_items() const;
private:
    std::map<uint8_t, std::pair<CapabilityItemType, std::any&>> _ordered_items;
};


#endif  // RANK_PRELUDE_REQUESTINGCAPABILITIES_H
