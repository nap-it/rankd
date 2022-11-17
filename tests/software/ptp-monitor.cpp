#include <iostream>
#include <map>

#include "time-lib.h"

int main() {
  auto info = get_ptp_capabilities("eth0");

  for (auto [key, value] : info) {
    std::cout << key << ":   (with " << value.size() << " values)" << std::endl;
    for (const auto& element : value) {
      std::cout << "\t" << element << std::endl;
    }
  }

  std::cout << "The device has capabilities of PTP? ";
  auto has = capability_level_in(info);
  switch (has) {
  case PTPCapability::HARDWARE:
    std::cout << "Hardware" << std::endl; break;
  case PTPCapability::SOFTWARE_ONLY:
    std::cout << "Software" << std::endl; break;
  case PTPCapability::UNCAPABLE:
    std::cout << "No" << std::endl; break;
  }

  std::cout << std::endl;

  debug_all_interfaces_ptp_capabilities();

  return 0;
}