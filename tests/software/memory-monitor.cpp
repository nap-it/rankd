#include <iostream>
#include <thread>

#include "structs/memory.h"

using namespace std;

int main() {
  Memory memory;
  memory.snap();

  cout << "Memory usage is: (Total/Free/Available)" << endl;
  while (true) {
    cout << ". " << memory.total().value()/1048576.0 << " / " << memory.free().value()/1048576.0 << " / " << memory.available().value()/1048576.0 << endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    memory.snap();
  }

  return 0;
}