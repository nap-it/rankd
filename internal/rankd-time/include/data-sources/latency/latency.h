#ifndef RANKDTIME_LATENCY_H
#define RANKDTIME_LATENCY_H

#include <ostream>

#include "standards.h"

class Latency {
public:
    Latency();
    void snap();
    friend std::ostream& operator<<(std::ostream& os, const Latency& configuration);
private:

};

#endif //RANKDTIME_LATENCY_H