#ifndef RANKDTIME_SYNCHRONIZATION_H
#define RANKDTIME_SYNCHRONIZATION_H

#include <ostream>

class Synchronization {
public:
    Synchronization();
    void snap();
    friend std::ostream& operator<<(std::ostream& os, const Synchronization& configuration);
private:

};

#endif //RANKDTIME_SYNCHRONIZATION_H