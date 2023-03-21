#include "data-sources/storage.h"

Storage::Storage() {
  _devices = std::map<std::string, Device>();

  snap();
}

void Storage::snap() {
  std::string partitions_filename("/proc/partitions");
  std::ifstream partitions_file(partitions_filename);

  // Trash the first two lines.
  std::string disposable_string;
  //partitions_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  //partitions_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  getline(partitions_file, disposable_string);
  getline(partitions_file, disposable_string);

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

rapidjson::Document Storage::json() const {
  // Create a JSON document.
  rapidjson::Document json_document;
  json_document.SetObject();
  auto& allocator = json_document.GetAllocator();

  // Create temporary value.
  rapidjson::Value value;

  value.SetObject();
  json_document.AddMember("root-device", value, allocator);
  value.SetString(rapidjson::GenericStringRef(_root_storage->path.c_str()));
  json_document["root-device"].AddMember("path", value, allocator);
  value.SetString(rapidjson::GenericStringRef(_root_storage->filesystem.c_str()));
  json_document["root-device"].AddMember("filesystem", value, allocator);
  value.SetUint64(_root_storage->size);
  json_document["root-device"].AddMember("size", value, allocator);
  value.SetUint64(_root_storage->used);
  json_document["root-device"].AddMember("used", value, allocator);
  value.SetUint64(_root_storage->available);
  json_document["root-device"].AddMember("available", value, allocator);
  value.SetDouble(_root_storage->use_percentage);
  json_document["root-device"].AddMember("use-percentage", value, allocator);
  value.SetString(rapidjson::GenericStringRef(_root_storage->mounted_on.c_str()));
  json_document["root-device"].AddMember("mounted-on", value, allocator);
  value.SetObject();
  json_document["root-device"].AddMember("iops", value, allocator);
  value.SetUint64(_root_storage->iops.reads_completed);
  json_document["root-device"]["iops"].AddMember("reads-completed", value, allocator);
  value.SetUint64(_root_storage->iops.reads_merged);
  json_document["root-device"]["iops"].AddMember("reads-merged", value, allocator);
  value.SetUint64(_root_storage->iops.sectors_read);
  json_document["root-device"]["iops"].AddMember("sectors-read", value, allocator);
  value.SetUint64(_root_storage->iops.ms_spent_reading);
  json_document["root-device"]["iops"].AddMember("ms-spent-reading", value, allocator);
  value.SetUint64(_root_storage->iops.writes_completed);
  json_document["root-device"]["iops"].AddMember("writes-completed", value, allocator);
  value.SetUint64(_root_storage->iops.writes_merged);
  json_document["root-device"]["iops"].AddMember("writes-merged", value, allocator);
  value.SetUint64(_root_storage->iops.sectors_written);
  json_document["root-device"]["iops"].AddMember("sectors-written", value, allocator);
  value.SetUint64(_root_storage->iops.ms_spent_writing);
  json_document["root-device"]["iops"].AddMember("ms-spent-writing", value, allocator);
  value.SetUint64(_root_storage->iops.io_in_progress);
  json_document["root-device"]["iops"].AddMember("io-in-progress", value, allocator);
  value.SetUint64(_root_storage->iops.ms_doing_io);
  json_document["root-device"]["iops"].AddMember("ms-doing-io", value, allocator);

  value.SetObject();
  json_document.AddMember("devices", value, allocator);
  for (const auto& [name, device] : _devices) {
    value.SetObject();
    json_document["devices"].AddMember(rapidjson::GenericStringRef(name.c_str()), value, allocator);
    value.SetString(rapidjson::GenericStringRef(device.path.c_str()));
    json_document["devices"][name.c_str()].AddMember("path", value, allocator);
    value.SetString(rapidjson::GenericStringRef(device.filesystem.c_str()));
    json_document["devices"][name.c_str()].AddMember("filesystem", value, allocator);
    value.SetUint64(device.size);
    json_document["devices"][name.c_str()].AddMember("size", value, allocator);
    value.SetUint64(device.used);
    json_document["devices"][name.c_str()].AddMember("used", value, allocator);
    value.SetUint64(device.available);
    json_document["devices"][name.c_str()].AddMember("available", value, allocator);
    value.SetDouble(device.use_percentage);
    json_document["devices"][name.c_str()].AddMember("use-percentage", value, allocator);
    value.SetString(rapidjson::GenericStringRef(device.mounted_on.c_str()));
    json_document["devices"][name.c_str()].AddMember("mounted-on", value, allocator);
    value.SetObject();
    json_document["devices"].AddMember("iops", value, allocator);
    value.SetUint64(device.iops.reads_completed);
    json_document["devices"]["iops"].AddMember("reads-completed", value, allocator);
    value.SetUint64(device.iops.reads_merged);
    json_document["devices"]["iops"].AddMember("reads-merged", value, allocator);
    value.SetUint64(device.iops.sectors_read);
    json_document["devices"]["iops"].AddMember("sectors-read", value, allocator);
    value.SetUint64(device.iops.ms_spent_reading);
    json_document["devices"]["iops"].AddMember("ms-spent-reading", value, allocator);
    value.SetUint64(device.iops.writes_completed);
    json_document["devices"]["iops"].AddMember("writes-completed", value, allocator);
    value.SetUint64(device.iops.writes_merged);
    json_document["devices"]["iops"].AddMember("writes-merged", value, allocator);
    value.SetUint64(device.iops.sectors_written);
    json_document["devices"]["iops"].AddMember("sectors-written", value, allocator);
    value.SetUint64(device.iops.ms_spent_writing);
    json_document["devices"]["iops"].AddMember("ms-spent-writing", value, allocator);
    value.SetUint64(device.iops.io_in_progress);
    json_document["devices"]["iops"].AddMember("io-in-progress", value, allocator);
    value.SetUint64(device.iops.ms_doing_io);
    json_document["devices"]["iops"].AddMember("ms-doing-io", value, allocator);
  }

  return json_document;
}

std::ostream& operator<<(std::ostream& os, const Storage& storage) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  auto json_document = storage.json();
  json_document.Accept(writer);

  os << buffer.GetString();

  return os;
}
