#include <iostream>
#include <thread>

#include "comp-lib.h"

using namespace std;

int main() {
  CPU cpu = CPU();
  cpu.snap();

  cout << "Details on the current CPU:" << endl;
  cout << "Vendor ID: " << cpu.vendor_id() << endl;
  cout << "Model Name: " << cpu.model_name() << endl;
  cout << "Model: " << cpu.model() << endl;
  cout << "Family: " << cpu.family() << endl;
  cout << "Stepping: " << cpu.stepping() << endl;
  cout << "Microcode: " << cpu.microcode() << endl;
  cout << "Flags: ";
  for (auto& flag : cpu.flags()) {
    cout << flag << ", ";
  }
  cout << endl;

  cout << "CPU stats are: (System/User/Nice/IRQs/SoftIRQs/IOwait/IDLE/Steal)"
       << endl;
  while (true) {
    //cout << "· " << cpu.snapshot().system << "/" << cpu.snapshot().user << "/"
    //     << cpu.snapshot().nice << "/" << cpu.snapshot().irqs << "/"
    //     << cpu.snapshot().softirqs << "/" << cpu.snapshot().iowait << "/"
    //     << cpu.snapshot().idle << "/" << cpu.snapshot().steal << endl;
    cout << (cpu.snapshot().in_use * 1000000)/2.4 << " %" << endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    cpu.snap();
  }

  return 0;
}