#ifndef RANK_PRELUDE_CAPABILITIES_H
#define RANK_PRELUDE_CAPABILITIES_H

#include <optional>

class Capabilities {
public:
    // Marshalling features.
    static const char* marshal();
    static void unmarshal(const char* marshalled);

    // Getters.
    std::optional<double> bandwidth() const;

    // Setters.
    void bandwidth(double value);

    // Operators.
    bool operator<(const Capabilities& prototype) const;
private:
    std::optional<double> _bandwidth;
};


#endif  // RANK_PRELUDE_CAPABILITIES_H
