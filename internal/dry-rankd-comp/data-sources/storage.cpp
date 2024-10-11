#include "data-sources/storage.h"

static unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::mt19937 range(seed);
static std::uniform_int_distribution<std::mt19937::result_type> full_randomizer(0, RAND_MAX);

Storage::Storage() {
    // Initialize storage devices.
    int number_of_devices = (full_randomizer(range) % 3) + 1;
    for (int device_index = 0; device_index != number_of_devices; device_index++) {
        Device device;
        device.path = "path/of/the/storage";
        device.filesystem = "ExFAT";
        device.size = (full_randomizer(range) % 1000) + 128;
        device.available = (full_randomizer(range) % 100) + 1;
        device.use_percentage = (1 - ((device.available / 100.0) * device.size)) * 100.0;
        device.used = device.use_percentage * device.size;
        device.mounted_on = "/mnt/" + std::to_string(number_of_devices);
        device.iops = IOps();

        _devices[device.mounted_on] = device;
    }
    _root_storage = &_devices["/mnt/0"];

    // Take a snapshot of the system.
    snap();
}

void Storage::snap() {
    for (auto& [path, device]: _devices) {
        unsigned long reads_incr = full_randomizer(range) % 100;
        unsigned long writes_incr = full_randomizer(range) % 100;
        unsigned long used_incr = full_randomizer(range) % 10000;

        device.iops.reads_completed += reads_incr;
        device.iops.writes_completed += writes_incr;
        device.used += used_incr;
        if (device.used > device.size) device.used = device.size;

        device.available = device.size - device.used;
        device.use_percentage = (static_cast<double>(device.used) / device.size) * 100;
    }
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