#include "comp-lib.h"

#include <iomanip>
#include <iostream>
#include <thread>

using namespace std;

int main() {
  Storage storage = Storage();
  auto devices = storage.devices();

  cout << left << setw(16) << "Filesystem" << " " << right << setw(4) << "Size" << " " << setw(4) << "Used" << " " << setw(5) << "Avail" << " " << setw(4) << "Use%" << " " << left << setw(10) << "Type" << " " << "Mounted on" << endl;
  for (auto& [_, device] : devices) {
    cout << left << setw(16) << device.path << " "
         << right << setw(4) << (device.size < 1000000 ? to_string(device.size/1024)+"K" : device.size < 1000000000 ? to_string(device.size/1048576)+"M" : to_string(device.size/(1048576*1024))+"G") << " "
         << setw(4) << (device.used < 1000000 ? to_string(device.used/1024)+"K" : device.used < 1000000000 ? to_string(device.used/1048576)+"M" : to_string(device.used/(1048576*1024))+"G") << " "
         << setw(5) << (device.available < 1000000 ? to_string(device.available/1024)+"K" : device.available < 1000000000 ? to_string(device.available/1048576)+"M" : to_string(device.available/(1048576*1024))+"G") << " "
         << setw(4) << to_string(static_cast<int>(device.use_percentage))+"%" << left << " "
         << setw(10) << device.filesystem << " "
         << device.mounted_on << endl;
  }
  cout << endl;

  auto root_device = storage.root_device();
  cout << "The root filesystem is in the " << root_device->path << " device." << endl;
  cout << "Its used percentage is of " << root_device->use_percentage << "%." << endl;
  cout << endl;

  cout << left << setw(10) << "Reads Made" << " " << setw(10) << "Reading ms" << " " << setw(10) << "Write Made" << " " << setw(10) << "Writin ms" << endl;
  while (true) {
    storage.snap();
    cout << left << setw(10) << root_device->iops.reads_completed << " "
         << setw(10) << root_device->iops.ms_spent_reading << " "
         << setw(10) << root_device->iops.writes_completed << " "
         << setw(10) << root_device->iops.ms_spent_writing << endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}