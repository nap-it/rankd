#ifndef RANKD_OS_H
#define RANKD_OS_H

#define RAPIDJSON_HAS_STDSTRING 1

#include <pwd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include <chrono>
#include <map>
#include <numeric>
#include <string>

#include "pfs/procfs.hpp"

// RapidJSON inclusions.
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"

enum class ProcessStatus {
  Running,
  Sleeping,
  DiskSleep,
  Stopped,
  TracingStop,
  Zombie,
  Dead,
  Wakekill,
  Waking,
  Parked,
  Idle
};

ProcessStatus parse_state_type(const pfs::task_state& state);
std::string get_state_name(const ProcessStatus& state);
std::string get_state_name(const pfs::task_state& state);

struct Process {
  int id;
  std::string user;
  double cpu_usage;
  double memory_usage;
  ProcessStatus status;
  std::chrono::duration<long, std::micro> time;
  std::string command;
  int threads;
};

struct Kernel {
  std::string sysname;
  std::string nodename;
  std::string release;
  std::string version;
  std::string machine;
};

std::string get_username_from_uid(int uid);

class OperativeSystem {
public:
  OperativeSystem();
  void snap();
  [[nodiscard]] const Kernel& kernel() const;
  [[nodiscard]] const std::chrono::seconds &uptime() const;
  [[nodiscard]] double load_1m() const;
  [[nodiscard]] double load_5m() const;
  [[nodiscard]] double load_15m() const;
  [[nodiscard]] const std::map<unsigned int, Process> &processes() const;
  [[nodiscard]] rapidjson::Document json() const;
  friend std::ostream& operator<<(std::ostream& os, const OperativeSystem& operative_system);
private:
  Kernel _kernel;
  std::chrono::seconds _uptime{};
  double _load1m;
  double _load5m;
  double _load15m;
  std::map<unsigned int, Process> _processes;
};

const float LOAD = 1.f / (1 << SI_LOAD_SHIFT);

#endif // RANKD_OS_H
