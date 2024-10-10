#include "data-sources/cpu.h"

#include <iostream>

CPU::CPU() {
    // Initialize all the variables for future snapping.
    // TODO

    // Take a snapshot of the system, CPU-wise.
    snap();
}

uint8_t CPU::identifier() const {
    return _identifier;
}

ByteOrder CPU::byte_order() const {
    return _byte_order;
}

std::string CPU::vendor_id() const {
    return _vendor_id ? _vendor_id.value() : "";
}

std::string CPU::family() const {
    return _family ? _family.value() : "";
}

std::string CPU::model() const {
    return _model ? _model.value() : "";
}

std::string CPU::model_name() const {
    return _model_name ? _model_name.value() : "";
}

std::string CPU::stepping() const {
    return _stepping ? _stepping.value() : "";
}

std::string CPU::microcode() const {
    return _microcode ? _microcode.value() : "";
}

const std::set<std::string>& CPU::flags() const {
    return _flags;
}

const std::set<std::string>& CPU::bugs() const {
    return _bugs;
}

double CPU::bogomips() const {
    return _bogomips;
}

const std::map<int, CPUCore>& CPU::cores() const {
    return _cores;
}

const CPUStats& CPU::snapshot() const {
    return _snapshot;
}

void CPU::snap() {
    // Update all the values for a snap.
    // TODO
}
