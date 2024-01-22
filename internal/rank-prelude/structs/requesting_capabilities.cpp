#include "structs/requesting_capabilities.h"

void RequestingCapabilities::add_item(const std::pair<CapabilityItemType, std::any&>& item, uint8_t place) {
    _ordered_items.insert({place, item});
}

void RequestingCapabilities::downgrade_item(uint8_t place, uint8_t to_place) {
    // TODO
    _ordered_items.at(to_place) = _ordered_items.at(place);
    _ordered_items.erase(place);
}

void RequestingCapabilities::upgrade_item(uint8_t place, uint8_t to_place) {
    // TODO
    _ordered_items.at(to_place) = _ordered_items.at(place);
    _ordered_items.erase(place);
}

const std::pair<CapabilityItemType, std::any&>& RequestingCapabilities::get_item(uint8_t place) {
    return _ordered_items.at(place);
}

void RequestingCapabilities::remove_item(uint8_t place) {
    _ordered_items.erase(place);
}

std::map<uint8_t, std::pair<CapabilityItemType, std::any&>> RequestingCapabilities::ordered_items() const {
    return _ordered_items;
}