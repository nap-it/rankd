#include "storage.h"

Storage::Storage() {
  _devices = std::map<std::string, Device>();

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

  std::vector<std::string> partition_names;

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
        partition_names.push_back(partition);
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
    if (partition_name.empty()) {
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
    double used_percentage = (static_cast<double>(used) / static_cast<double>(total)) * 100.0;

    Device device;
    device.path = partition_name;
    device.available = available;
    device.size = total;
    device.used = used;
    device.use_percentage = used_percentage;
    device.mounted_on = partition_info.first.empty() ? "Unmounted" : partition_info.first;
    device.filesystem = partition_info.second.empty() ? "unknown" : partition_info.second;

    _devices[device.path] = device;

    if (device.mounted_on == "/") {
      _root_storage = &_devices[device.path];
    }
  }

  // Get I/O operation statistics.
  std::ifstream iops_file("/proc/diskstats");

  while (!iops_file.eof()) {
    int integer_value;
    iops_file >> integer_value;
    iops_file >> integer_value;

    std::string partition_name;
    iops_file >> partition_name;

    // If this partition name is one of our considered ones, do something.
    if (std::find(partition_names.begin(), partition_names.end(), partition_name) != std::end(partition_names)) {
      IOps iops{};
      iops_file >> iops.reads_completed;
      iops_file >> iops.reads_merged;
      iops_file >> iops.sectors_read;
      iops_file >> iops.ms_spent_reading;
      iops_file >> iops.writes_completed;
      iops_file >> iops.writes_merged;
      iops_file >> iops.sectors_written;
      iops_file >> iops.ms_spent_writing;
      iops_file >> iops.io_in_progress;
      iops_file >> iops.ms_doing_io;
      iops_file >> iops.weighted_ms_doing_io;
      _devices["/dev/"+partition_name].iops = iops;
    }
    std::string temporary_string;
    getline(iops_file, temporary_string);
  }

  iops_file.close();
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