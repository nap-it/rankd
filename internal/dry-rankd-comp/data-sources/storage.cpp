#include "data-sources/storage.h"

Storage::Storage() {
    // Initialize all the variables for future snapping.
    // TODO

    // Take a snapshot of the system.
    snap();
}

void Storage::snap() {
    // TODO
}

const std::map<std::string, Device> &Storage::devices() const {
  return _devices;
}

const Device& Storage::root_device_snap() const {
  return *_root_storage;
}

const Device* Storage::root_device() const {
  return _root_storage;
}