#ifndef RANKDCOMP_DARWIN_CPU_H
#define RANKDCOMP_DARWIN_CPU_H

#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include <sys/sysctl.h>

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
  [[nodiscard]] ByteOrder byte_order() const;
  [[nodiscard]] const std::string &vendor_id() const;
  [[nodiscard]] const int &family() const;
  [[nodiscard]] const int &model() const;
  [[nodiscard]] const std::string &model_name() const;
  [[nodiscard]] const std::string &stepping() const;
  [[nodiscard]] const std::string &microcode() const;
  [[nodiscard]] const std::set<std::string> &flags() const;
  [[nodiscard]] const int &cores() const;
  [[nodiscard]] const CPUStats &snapshot() const;

private:
  /* Information */
  ByteOrder _byte_order;  // TODO Get this information.
  std::string _vendor_id;
  int _family;
  int _model;
  std::string _model_name;
  std::string _stepping;
  std::string _microcode;
  std::set<std::string> _flags;
  int _cores;

  /* Statistics */
  CPUStats _snapshot{};
};

#endif // RANKDCOMP_DARWIN_CPU_H
