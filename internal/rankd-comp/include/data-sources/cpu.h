#ifndef RANKDCOMP_LINUX_CPU_H
#define RANKDCOMP_LINUX_CPU_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <map>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <string>

#define RAPIDJSON_HAS_STDSTRING 1

// RapidJSON inclusions.
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"

#include "pfs/procfs.hpp"
#include "utils/strings.h"

struct CPUCore {
  /// Identifier of the CPU core.
  uint8_t identifier{};
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
  /// In use percentage.
  double in_use;
  /// Total percentage.
  double total;
};

class CPU {
public:
  CPU();
  void snap();
  [[nodiscard]] uint8_t identifier() const;
  [[nodiscard]] ByteOrder byte_order() const;
  [[nodiscard]] std::string vendor_id() const;
  [[nodiscard]] std::string family() const;
  [[nodiscard]] std::string model() const;
  [[nodiscard]] std::string model_name() const;
  [[nodiscard]] std::string stepping() const;
  [[nodiscard]] std::string microcode() const;
  [[nodiscard]] const std::set<std::string> &flags() const;
  [[nodiscard]] const std::set<std::string> &bugs() const;
  [[nodiscard]] double bogomips() const;
  [[nodiscard]] const std::map<int, CPUCore> &cores() const;
  [[nodiscard]] const CPUStats &snapshot() const;
  void enable_json_output();
  void disable_json_output();
  [[nodiscard]] rapidjson::Document json() const;
  friend std::ostream& operator<<(std::ostream& os, const CPU& cpu);

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
  bool _json_formatted_output = false;
};

#endif // RANKDCOMP_LINUX_CPU_H
