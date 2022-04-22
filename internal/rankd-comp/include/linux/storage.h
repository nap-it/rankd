#ifndef RANKD_STORAGE_H
#define RANKD_STORAGE_H

#include <mntent.h>
#include <sys/statvfs.h>

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "utils/strings.h"

struct Device {
  std::string path;
  std::string filesystem;
  unsigned long int size;
  unsigned long int used;
  unsigned long int available;
  double use_percentage;
  std::string mounted_on;
};

class Storage {
public:
  Storage();
  void snap();
  [[nodiscard]] const std::vector<Device>& devices() const;
private:
  std::vector<Device> _devices;
};

#endif // RANKD_STORAGE_H
