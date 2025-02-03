#ifndef RANK_PRELUDE_CURRENT_CAPABILITIES_H
#define RANK_PRELUDE_CURRENT_CAPABILITIES_H

#include <optional>
#include <sstream>

#ifdef FROM_SIMUZILLA
#include "dry-comp-lib.h"
#include "dry-net-lib.h"
//#include "dry-time-lib.h"
#else
#include "comp-lib.h"
#include "net-lib.h"
#include "time-lib.h"
#endif

#include "structs/capabilities.h"

class CurrentCapabilities : public Capabilities {
public:
#ifdef FROM_SIMUZILLA
    CurrentCapabilities(const std::function<const std::vector<int>*()>& topology, unsigned int own_address) {
        _cpu = new CPU();
        _memory = new Memory();
        _os = new OperativeSystem();
        _sensors = new Sensors();
        _storage = new Storage();
        _interfaces = new NetworkDevices(topology, own_address);
        _neighbors = new NetworkNeighbors(topology, own_address);
        _routes = new NetworkRoutes(topology, own_address);
        //_time_configuration = new Configuration();
        //_time_latency = new Latency();
        //_time_reliability = new Reliability();
        //_time_synchronization = new Synchronization();
    }
#endif
    void update();

    uint bare_metal_cpu_cores();
    uint bare_metal_memory();

    float current_cpu_cores();
    float current_memory();

    std::string display() const;
private:
    /// Computational Resources.
    CPU* _cpu{};
    Memory* _memory{};
    OperativeSystem* _os{};
    Sensors* _sensors{};
    Storage* _storage{};

    /// Network Resources.
    NetworkDevices* _interfaces{};
    NetworkNeighbors* _neighbors{};
    NetworkRoutes* _routes{};

    /// Time Resources.
    //Configuration* _time_configuration{};
    //Latency* _time_latency{};
    //Reliability* _time_reliability{};
    //Synchronization* _time_synchronization{};
};


#endif  // RANK_PRELUDE_CURRENT_CAPABILITIES_H
