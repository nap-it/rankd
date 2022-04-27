#ifndef RANKD_STORAGE_H
#define RANKD_STORAGE_H

#include <mntent.h>
#include <sys/statvfs.h>

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "utils/strings.h"

// https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats
struct IOps {
  unsigned long reads_completed;
  unsigned long reads_merged;
  unsigned long sectors_read;
  unsigned long ms_spent_reading;
  unsigned long writes_completed;
  unsigned long writes_merged;
  unsigned long sectors_written;
  unsigned long ms_spent_writing;
  unsigned long io_in_progress;
  unsigned long ms_doing_io;
  unsigned long weighted_ms_doing_io;
};

struct Device {
  std::string path;
  std::string filesystem;
  unsigned long int size;
  unsigned long int used;
  unsigned long int available;
  double use_percentage;
  std::string mounted_on;
  IOps iops;
};

class Storage {
public:
  Storage();
  void snap();
  [[nodiscard]] const std::map<std::string, Device>& devices() const;
  [[nodiscard]] const Device& root_device_snap() const;
  [[nodiscard]] const Device* root_device() const;
private:
  std::map<std::string, Device> _devices;
  Device* _root_storage;
};

#endif // RANKD_STORAGE_H
