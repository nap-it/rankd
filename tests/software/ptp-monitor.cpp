#include <iostream>
#include <map>

#include "time-lib.h"

int main() {
  auto info = get_ptp_capabilities("enp2s0");

  for (auto [key, value] : info) {
    std::cout << key << ":   (with " << value.size() << " values)" << std::endl;
    for (const auto& element : value) {
      std::cout << "\t" << element << std::endl;
    }
  }

  return 0;
}