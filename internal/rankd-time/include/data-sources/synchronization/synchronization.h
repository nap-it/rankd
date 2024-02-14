#ifndef RANKDTIME_SYNCHRONIZATION_H
#define RANKDTIME_SYNCHRONIZATION_H

#include <ostream>

class Synchronization {
public:
    Synchronization();
    void snap();
    friend std::ostream& operator<<(std::ostream& os, const Synchronization& configuration);
private:
    bool _json_formatted_output = false;
};

#endif //RANKDTIME_SYNCHRONIZATION_H