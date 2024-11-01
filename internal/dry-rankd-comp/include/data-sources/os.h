#ifndef DRYRANKD_OS_H
#define DRYRANKD_OS_H

#include <pwd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include <chrono>
#include <iomanip>
#include <map>
#include <numeric>
#include <random>
#include <string>

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

ProcessStatus random_status();

std::string random_user();

std::string random_command();

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
  friend std::ostream& operator<<(std::ostream& os, const OperativeSystem& operative_system);
private:
  Kernel _kernel;
  std::chrono::seconds _uptime{};
  double _starting_time_point;
  double _load1m;
  double _load5m;
  double _load15m;
  std::map<unsigned int, Process> _processes;
};

const float LOAD = 1.f / (1 << SI_LOAD_SHIFT);

#endif // DRYRANKD_OS_H
