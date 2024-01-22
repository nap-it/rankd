#include "structs/capabilities.h"

std::map<CapabilityItemType, std::any> Capabilities::items() const {
    return _items;
}

bool Capabilities::operator<(const Capabilities& prototype) const {

}

bool Capabilities::operator==(const Capabilities& prototype) const {
    if (_items.size() != prototype._items.size()) {
        return false;
    }

    for (const auto& _item : _items) {
        switch (_item.first) {
            case CapabilityItemType::UNSPECIFIED:
                if (prototype.items().count(CapabilityItemType::UNSPECIFIED) == 0) {
                    return false;
                }
                break;
            case CapabilityItemType::TIME_TAS:
                if (prototype.items().count(CapabilityItemType::TIME_TAS) == 0) {
                    return false;
                }
                // TODO.
                break;
            case CapabilityItemType::TIME_CBS:
                if (prototype.items().count(CapabilityItemType::TIME_CBS) == 0) {
                    return false;
                }
                // TODO.
                break;
            case CapabilityItemType::NET_BANDWIDTH:
                if (prototype.items().count(CapabilityItemType::NET_BANDWIDTH) == 0) {
                    return false;
                }
                // TODO.
                break;
            case CapabilityItemType::NET_DDS:
                if (prototype.items().count(CapabilityItemType::NET_DDS) == 0) {
                    return false;
                }
                // TODO.
                break;
            case CapabilityItemType::COMP_CPU:
                if (prototype.items().count(CapabilityItemType::COMP_CPU) == 0) {
                    return false;
                }
                // TODO.
                break;
            case CapabilityItemType::COMP_MEMORY:
                if (prototype.items().count(CapabilityItemType::COMP_MEMORY) == 0) {
                    return false;
                }
                // TODO.
                break;
        }
    }

    return true;
}

