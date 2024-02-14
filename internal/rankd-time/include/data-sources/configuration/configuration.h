#ifndef RANKDTIME_CONFIGURATION_H
#define RANKDTIME_CONFIGURATION_H

#include <ostream>

class Configuration {
public:
    Configuration();
    void snap();
    friend std::ostream& operator<<(std::ostream& os, const Configuration& configuration);
private:
    bool _json_formatted_output = false;
};

#endif //RANKDTIME_CONFIGURATION_H
