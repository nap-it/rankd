#include "structs/current_capabilities.h"

void CurrentCapabilities::update() {
    // Update computational resources.
    _cpu->snap();
    _memory->snap();
    _os->snap();
    _sensors->snap();
    _storage->snap();

    // Update network resources.
    _interfaces->snap();
    _neighbors->snap();
    _routes->snap();

    // Update time resources.
    //_time_configuration->snap();
    //_time_latency->snap();
    //_time_reliability->snap();
    //_time_synchronization->snap();
}

uint CurrentCapabilities::bare_metal_cpu_cores() {
    _cpu->snap();

    return _cpu->cores().size();
}

uint CurrentCapabilities::bare_metal_memory() {
    _memory->snap();

    return _memory->total();
}

float CurrentCapabilities::current_cpu_cores() {
    _cpu->snap();

    return 1.0; //_cpu->cores().size();
}

float CurrentCapabilities::current_memory() {
    _memory->snap();

    return _memory->available() / _memory->total();
}

std::string CurrentCapabilities::display() const {
    std::stringstream display_stream;

    // Handle computational resources.
    display_stream << "{C: ";
    display_stream << "cpu_cores: " << std::to_string(_cpu->cores().size()) << "/" << std::to_string(_cpu->cores().size()) << " ";
    display_stream << "mem_avail: " << std::to_string(_memory->available()) << "/" << std::to_string(_memory->total()) << " ";
    display_stream << "} ";

    return display_stream.str();
}