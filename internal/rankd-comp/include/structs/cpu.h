#ifndef RANKD_STRUCTS_CPU_H
#define RANKD_STRUCTS_CPU_H

#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>

class CPUCore {
private:
  uint8_t _identifier;
  std::optional<int> _frequency;
  std::optional<size_t> _cache_size;
  std::optional<bool> _has_fpu;
};

enum ByteOrder {
  LITTLE,
  BIG
};

struct CPUStats {
  /// Processes executing in kernel mode.
  double system;
  /// Normal processes executing in user mode.
  double user;
  /// Niced processes executing in user mode.
  double nice;
  /// Servicing interrupts.
  double irqs;
  /// Servicing Soft IRQs.
  double softirqs;
  /// Waiting for I/O to complete.
  double iowait;
  /// Waiting for something to happen.
  double idle;
  /// Time spent in other OSes when running in a virtualized environment.
  double steal;
};

class CPU {
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
  std::optional<std::set<std::string>> _bugs;
  double _bogomips;
  std::map<int, CPUCore> _cores;

  /* Statistics */
  double _cpu_busy;
  CPUStats _snapshot;
};

#endif // RANKD_STRUCTS_CPU_H
