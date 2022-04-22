#include <iostream>
#include <thread>

#include "memory.h"

using namespace std;

int main() {
  Memory memory;
  memory.snap();

  cout << "Memory usage is: (Total/Free/Available)" << endl;
  while (true) {
    cout << ". " << memory.total()/1048576.0 << " / " << memory.free()/1048576.0 << " / " << memory.available()/1048576.0 << " / " << "with " << memory.oom_killer_invocations() << " OOM Killer invocations." << endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    memory.snap();
  }

  return 0;
}