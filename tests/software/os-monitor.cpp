#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

#include <sys/sysinfo.h>

#include "comp-lib.h"

using namespace std;

int main() {
  struct sysinfo system_info;

  const float LOAD = 1.f / (1 << SI_LOAD_SHIFT);

  auto os = OperativeSystem();

  cout << "System information:" << endl;
  cout << "\t" << "Kernel" << endl;
  cout << "\t" << "- System name: " << os.kernel().sysname << endl;
  cout << "\t" << "- Node name: " << os.kernel().nodename << endl;
  cout << "\t" << "- Release: " << os.kernel().release << endl;
  cout << "\t" << "- Version: " << os.kernel().version << endl;
  cout << "\t" << "- Machine: " << os.kernel().machine << endl;
  cout << endl;

  while (true) {
    os.snap();

    cout << "Uptime: " << os.uptime().count() << " seconds." << endl;
    cout << "Loads (1m/5m/15m): " << os.load_1m() << "/" << os.load_5m() << "/" << os.load_15m() << endl;
    cout << "Processes:" << endl;
    cout << left << setw(20) << "User" << " " << "PID" << " " << setw(10) << "Status" << " " << "Command" << endl;
    for (auto& [pid, process] : os.processes()) {
      cout << left << setw(20) << process.user << " " << pid << " " << setw(10) << get_state_name(process.status) << " " << process.command << endl;
    }
    cout << endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
