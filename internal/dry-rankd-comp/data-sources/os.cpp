#include "data-sources/os.h"

#include <iostream>

OperativeSystem::OperativeSystem() {
    // Initialize all the variables for future snapping.
    // TODO

    // Take a snapshot of the system.
    snap();
}

void OperativeSystem::snap() {
    // Update all the variables as in a snap.
}
const Kernel& OperativeSystem::kernel() const {
    return _kernel;
}

const std::chrono::seconds& OperativeSystem::uptime() const {
    return _uptime;
}

double OperativeSystem::load_1m() const {
    return _load1m;
}

double OperativeSystem::load_5m() const {
    return _load5m;
}

double OperativeSystem::load_15m() const {
    return _load15m;
}

const std::map<unsigned int, Process>& OperativeSystem::processes() const {
    return _processes;
}
