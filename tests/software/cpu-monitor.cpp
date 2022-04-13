#include <iostream>
#include <thread>

#include "structs/cpu.h"

using namespace std;

int main() {
  CPU cpu = CPU();
  cpu.snap();

  cout << "Details on the current CPU:" << endl;
  cout << "Vendor ID: " << cpu.vendor_id().value() << endl;
  cout << "Model Name: " << cpu.model().value() << endl;

  cout << "CPU stats are: (System/User/Nice/IRQs/SoftIRQs/IOwait/IDLE/Steal)"
       << endl;
  while (true) {
    cout << "· " << cpu.snapshot().system << "/" << cpu.snapshot().user << "/"
         << cpu.snapshot().nice << "/" << cpu.snapshot().irqs << "/"
         << cpu.snapshot().softirqs << "/" << cpu.snapshot().iowait << "/"
         << cpu.snapshot().idle << "/" << cpu.snapshot().steal << endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    cpu.snap();
  }

  return 0;
}