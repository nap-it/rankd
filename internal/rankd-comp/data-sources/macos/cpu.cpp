#include "cpu.h"
#include <mach/mach_host.h>

CPU::CPU() {
  size_t vector_length = 1024;
  size_t char_array_length = 1024;
  size_t integer_length = 1024;
  char collecting_string[char_array_length];
  char collecting_vector[vector_length];

  sysctlbyname("machdep.cpu.vendor", collecting_string, &char_array_length,
               nullptr, 0);
  _vendor_id = std::string(collecting_string);
  char_array_length = 1024;

  sysctlbyname("machdep.cpu.brand_string", collecting_string,
               &char_array_length, nullptr, 0);
  _model_name = std::string(collecting_string);
  char_array_length = 1024;

  sysctlbyname("machdep.cpu.family", &_family, &integer_length, nullptr, 0);
  integer_length = 1024;

  sysctlbyname("machdep.cpu.model", &_model, &integer_length, nullptr, 0);
  integer_length = 1024;

  sysctlbyname("machdep.cpu.stepping", &_stepping, &integer_length, nullptr, 0);
  integer_length = 1024;

  sysctlbyname("machdep.cpu.microcode_version", &_microcode, &integer_length,
               nullptr, 0);
  integer_length = 1024;

  sysctlbyname("machdep.cpu.core_count", &_cores, &integer_length, nullptr, 0);
  integer_length = 1024;

  sysctlbyname("machdep.cpu.features", collecting_vector, &vector_length,
               nullptr, 0);
  std::stringstream flags(collecting_vector);
  while (!flags.eof()) {
    std::string item;
    flags >> item;
    std::transform(item.begin(), item.end(), item.begin(),
                   [](char c) { return tolower(c); });
    _flags.insert(item);
  }
  vector_length = 1024;

  snap();
}

ByteOrder CPU::byte_order() const { return _byte_order; }

const std::string &CPU::vendor_id() const { return _vendor_id; }

const int &CPU::family() const { return _family; }

const int &CPU::model() const { return _model; }

const std::string &CPU::model_name() const { return _model_name; }

const int &CPU::stepping() const { return _stepping; }

const int &CPU::microcode() const { return _microcode; }

const std::set<std::string> &CPU::flags() const { return _flags; }

const int &CPU::cores() const { return _cores; }

void CPU::snap() {
  natural_t number_of_cpus = 0U;
  processor_cpu_load_info_data_t *cpu_load;
  mach_msg_type_number_t count;

  // Get information on the CPU cores.
  kern_return_t error = host_processor_info(host_t(mach_host_self()), PROCESSOR_CPU_LOAD_INFO,
                      &number_of_cpus, (processor_info_array_t *)&cpu_load,
                      &count);
  if (error != KERN_SUCCESS) {
    // TODO Return error code or handle the error.
    return;
  }

  for (unsigned int i = 0U; i < number_of_cpus; ++i) {
    if (_snapshot.idle != 0 && _snapshot.system != 0) {

    } else {
      _snapshot.system += cpu_load[(CPU_STATE_MAX * i)].cpu_ticks[CPU_STATE_SYSTEM];


      _snapshot.in_use = cpu_load[(CPU_STATE_MAX * i)].cpu_ticks[CPU_STATE_USER] + cpu_load[(CPU_STATE_MAX * i)].cpu_ticks[CPU_STATE_SYSTEM] + cpu_load[(CPU_STATE_MAX * i)].cpu_ticks[CPU_STATE_NICE];
      _snapshot.total = _snapshot.in_use + cpu_load[(CPU_STATE_MAX * i)].cpu_ticks[CPU_STATE_IDLE];
    }
  }


}