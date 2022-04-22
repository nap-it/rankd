#include "storage.h"

Storage::Storage() {
  _devices = std::vector<Device>();

  snap();
}

void Storage::snap() {
  std::string partitions_filename("/proc/partitions");
  std::ifstream partitions_file(partitions_filename);

  // Trash the first two lines.
  partitions_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  partitions_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  // Temporarily save the name of the found partitions. Type map of <mount, fs-type>.
  std::map<std::string, std::pair<std::string, std::string>> partitions;

  while (!partitions_file.eof()) {
    std::string line;
    getline(partitions_file, line);

    // Get last space position and get last word as substring.
    auto location = line.find_last_of(' ') + 1;
    if (location == std::string::npos) {
      continue;
    } else {
      std::string partition = line.substr(location);
      if (partition.empty()) {
        continue;
      } else {
        partitions["/dev/" + partition] = std::make_pair("", "");
      }
    }
  }

  partitions_file.close();

  // Get mounting points of each partition.
  FILE* drop_mount_info;
  struct mntent* entity;

  drop_mount_info = setmntent("/proc/mounts", "r");
  while ((entity = getmntent(drop_mount_info)) != nullptr) {
    if (partitions.count(std::string(entity->mnt_fsname)) > 0) {
      partitions.at(std::string(entity->mnt_fsname)) = std::make_pair(entity->mnt_dir, entity->mnt_type);
    }
  }

  // Get information on each partition.
  struct statvfs64 partition;

  for (auto& [partition_name, partition_info] : partitions) {
    if (partition_info.first.empty()) {
      continue;
    }

    int error_code = statvfs64(partition_info.first.c_str(), &partition);
    if (error_code != 0) {
      switch (error_code) { // TODO Handle these errors.
        case EACCES:
          break;
        case EBADF:
          break;
        case EFAULT:
          break;
        case EINTR:
          break;
        case EIO:
          break;
        case ELOOP:
          break;
        case ENAMETOOLONG:
          break;
        case ENOENT:
          break;
        case ENOMEM:
          break;
        case ENOSYS:
          break;
        case ENOTDIR:
          break;
        case EOVERFLOW:
          break;
        default:
          break;
      }
    }


    auto total = partition.f_blocks * partition.f_frsize;
    auto available = partition.f_bfree * partition.f_frsize;
    auto used = total - available;
    double used_percentage = (double)(used / total) * (double)100;

    Device device;
    device.path = partition_name;
    device.available = available;
    device.size = total;
    device.use_percentage = used_percentage;
    device.mounted_on = partition_info.first;
    device.filesystem = partition_info.second;

    _devices.push_back(device);
  }
}

const std::vector<Device> &Storage::devices() const {
  return _devices;
}
