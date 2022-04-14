#include "macos/cpu.h"

CPU::CPU() {
  size_t vector_length = 1024;
  size_t char_array_length = 256;
  size_t integer_length = 4;
  char collecting_string[char_array_length];
  char collecting_vector[vector_length];

  sysctlbyname("machdep.cpu.vendor", collecting_string, &char_array_length, nullptr, 0);
  _vendor_id = std::string(collecting_string);
  sysctlbyname("machdep.cpu.brand_string", collecting_string, &char_array_length, nullptr, 0);
  _model_name = std::string(collecting_string);

  sysctlbyname("machdep.cpu.family", &_family, &integer_length, nullptr, 0);
  sysctlbyname("machdep.cpu.model", &_model, &integer_length, nullptr, 0);
  sysctlbyname("machdep.cpu.stepping", &_stepping, &integer_length, nullptr, 0);
  sysctlbyname("machdep.cpu.microcode_version", &_microcode, &integer_length, nullptr, 0);
  sysctlbyname("machdep.cpu.core_count", &_cores, &integer_length, nullptr, 0);

  sysctlbyname("machdep.cpu.features", collecting_vector, &vector_length, nullptr, 0);
  std::stringstream flags(collecting_vector);
  while (!flags.eof()) {
    std::string item;
    flags >> item;
    std::transform(item.begin(), item.end(), item.begin(), [] (char c) { return tolower(c); });
    _flags.insert(item);
  }

  snap();
}

void CPU::snap() {
  
}