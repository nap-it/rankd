#ifndef DRYRANKDTIME_SYNCHRONIZATION_H
#define DRYRANKDTIME_SYNCHRONIZATION_H

#include <ostream>

class Synchronization {
public:
    Synchronization();
    void snap();
private:
    void snap_ptp();
};

#endif //DRYRANKDTIME_SYNCHRONIZATION_H