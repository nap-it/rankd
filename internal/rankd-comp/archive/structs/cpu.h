#ifndef RANKD_STRUCTS_CPU_H
#define RANKD_STRUCTS_CPU_H

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <sstream>

#include "pfs/procfs.hpp"

struct CPUCore {
  /// Identifier of the CPU core.
  uint8_t identifier;
  /// Frequency of the CPU core, in MHz.
  std::optional<double> frequency;
  /// Cache size of the CPU core, in Mbytes.
  std::optional<std::string> cache_size;
  /// Flag noticing if this CPU core has an FP unit.
  std::optional<bool> has_fpu;
};

enum ByteOrder {
  LITTLE,
  BIG
};

struct CPUStats {
  /// Processes executing in kernel mode.
  unsigned long long int system;
  /// Normal processes executing in user mode.
  unsigned long long int user;
  /// Niced processes executing in user mode.
  unsigned long long int nice;
  /// Servicing interrupts.
  unsigned long long int irqs;
  /// Servicing Soft IRQs.
  unsigned long long int softirqs;
  /// Waiting for I/O to complete.
  unsigned long long int iowait;
  /// Waiting for something to happen.
  unsigned long long int idle;
  /// Time spent in other OSes when running in a virtualized environment.
  unsigned long long int steal;
};

class CPU {
public:
  CPU();
  void snap();
  [[nodiscard]] uint8_t identifier() const;
  [[nodiscard]] ByteOrder byte_order() const;
  [[nodiscard]] const std::optional<std::string> &vendor_id() const;
  [[nodiscard]] const std::optional<std::string> &family() const;
  [[nodiscard]] const std::optional<std::string> &model() const;
  [[nodiscard]] const std::optional<std::string> &model_name() const;
  [[nodiscard]] const std::optional<std::string> &stepping() const;
  [[nodiscard]] const std::optional<std::string> &microcode() const;
  [[nodiscard]] const std::set<std::string> &flags() const;
  [[nodiscard]] const std::set<std::string> &bugs() const;
  [[nodiscard]] double bogomips() const;
  [[nodiscard]] const std::map<int, CPUCore> &cores() const;
  [[nodiscard]] const CPUStats &snapshot() const;

private:
  /* Information */
  uint8_t _identifier = 0;
  ByteOrder _byte_order;
  std::optional<std::string> _vendor_id;
  std::optional<std::string> _family;
  std::optional<std::string> _model;
  std::optional<std::string> _model_name;
  std::optional<std::string> _stepping;
  std::optional<std::string> _microcode;
  std::set<std::string> _flags;
  std::set<std::string> _bugs;
  double _bogomips;
  std::map<int, CPUCore> _cores;

  /* Statistics */
  CPUStats _snapshot{};
};

#endif // RANKD_STRUCTS_CPU_H
