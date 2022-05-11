#ifndef RANKD_OS_H
#define RANKD_OS_H

#include <pwd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include <chrono>
#include <map>
#include <numeric>
#include <string>

#include "pfs/procfs.hpp"

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
private:
  Kernel _kernel;
  std::chrono::seconds _uptime;
  std::map<unsigned int, Process> _processes;
};

#endif // RANKD_OS_H
