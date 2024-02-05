#ifndef RANKDTIME_RELIABILITY_H
#define RANKDTIME_RELIABILITY_H

#include <ostream>

class Reliability {
public:
    Reliability();
    void snap();
    friend std::ostream& operator<<(std::ostream& os, const Reliability& configuration);
private:

};

#endif //RANKDTIME_LATENCY_H