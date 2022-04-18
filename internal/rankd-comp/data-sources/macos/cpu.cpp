#include "cpu.h"

CPU::CPU() {
  size_t vector_length = 1024;
  size_t char_array_length = 1024;
  size_t integer_length = 1024;
  char collecting_string[char_array_length];
  char collecting_vector[vector_length];

  sysctlbyname("machdep.cpu.vendor", collecting_string, &char_array_length, nullptr, 0);
  _vendor_id = std::string(collecting_string);
  char_array_length = 1024;

  sysctlbyname("machdep.cpu.brand_string", collecting_string, &char_array_length, nullptr, 0);
  _model_name = std::string(collecting_string);
  char_array_length = 1024;

  sysctlbyname("machdep.cpu.family", &_family, &integer_length, nullptr, 0);
  integer_length = 1024;

  sysctlbyname("machdep.cpu.model", &_model, &integer_length, nullptr, 0);
  integer_length = 1024;

  sysctlbyname("machdep.cpu.stepping", &_stepping, &integer_length, nullptr, 0);
  integer_length = 1024;

  sysctlbyname("machdep.cpu.microcode_version", &_microcode, &integer_length, nullptr, 0);
  integer_length = 1024;

  sysctlbyname("machdep.cpu.core_count", &_cores, &integer_length, nullptr, 0);
  integer_length = 1024;


  sysctlbyname("machdep.cpu.features", collecting_vector, &vector_length, nullptr, 0);
  std::stringstream flags(collecting_vector);
  while (!flags.eof()) {
    std::string item;
    flags >> item;
    std::transform(item.begin(), item.end(), item.begin(), [] (char c) { return tolower(c); });
    _flags.insert(item);
  }
  vector_length = 1024;

  snap();
}

ByteOrder CPU::byte_order() const {
  return _byte_order;
}

const std::string &CPU::vendor_id() const {
  return _vendor_id;
}

const int &CPU::family() const {
  return _family;
}

const int &CPU::model() const {
  return _model;
}

const std::string &CPU::model_name() const {
  return _model_name;
}

const int &CPU::stepping() const {
  return _stepping;
}

const int &CPU::microcode() const {
  return _microcode;
}

const std::set<std::string> &CPU::flags() const {
  return _flags;
}

const int &CPU::cores() const {
  return _cores;
}


void CPU::snap() {

}