#include <iostream>
#include <thread>

#include "structs/network.h"

using namespace std;

int main() {
  Network network = Network();
  network.snap();
  network.snap();
  network.snap();

  return 0;
}