#ifndef RANK_PRELUDE_CURRENT_CAPABILITIES_H
#define RANK_PRELUDE_CURRENT_CAPABILITIES_H

#include <optional>

#include "structs/capabilities.h"

class CurrentCapabilities : public Capabilities {
public:
private:
    std::optional<double> _bandwidth_threshold;
};


#endif  // RANK_PRELUDE_CURRENT_CAPABILITIES_H
