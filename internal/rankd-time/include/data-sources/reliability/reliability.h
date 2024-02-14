#ifndef RANKDTIME_RELIABILITY_H
#define RANKDTIME_RELIABILITY_H

#include <ostream>

class Reliability {
public:
    Reliability();
    void snap();
    friend std::ostream& operator<<(std::ostream& os, const Reliability& configuration);
private:
    bool _json_formatted_output = false;
};

#endif //RANKDTIME_LATENCY_H