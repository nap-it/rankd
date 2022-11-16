#include "os.h"

#include <iostream>

ProcessStatus parse_state_type(const pfs::task_state& state) {
  switch (state) {
  case pfs::task_state::running:
    return ProcessStatus::Running;
  case pfs::task_state::sleeping:
    return ProcessStatus::Sleeping;
  case pfs::task_state::disk_sleep:
    return ProcessStatus::DiskSleep;
  case pfs::task_state::stopped:
    return ProcessStatus::Stopped;
  case pfs::task_state::tracing_stop:
    return ProcessStatus::TracingStop;
  case pfs::task_state::zombie:
    return ProcessStatus::Zombie;
  case pfs::task_state::dead:
    return ProcessStatus::Dead;
  case pfs::task_state::wakekill:
    return ProcessStatus::Wakekill;
  case pfs::task_state::waking:
    return ProcessStatus::Waking;
  case pfs::task_state::parked:
    return ProcessStatus::Parked;
  case pfs::task_state::idle:
    return ProcessStatus::Idle;
  }
}

std::string get_state_name(const ProcessStatus& state) {
  switch (state) {
  case ProcessStatus::Running:
    return "Running";
  case ProcessStatus::Sleeping:
    return "Sleeping";
  case ProcessStatus::DiskSleep:
    return "Disk Sleep";
  case ProcessStatus::Stopped:
    return "Stopped";
  case ProcessStatus::TracingStop:
    return "Tracing Stop";
  case ProcessStatus::Zombie:
    return "Zombie";
  case ProcessStatus::Dead:
    return "Dead";
  case ProcessStatus::Wakekill:
    return "Wakekill";
  case ProcessStatus::Waking:
    return "Waking";
  case ProcessStatus::Parked:
    return "Parked";
  case ProcessStatus::Idle:
    return "Idle";
  }
}

std::string get_state_name(const pfs::task_state& state) {
  switch (state) {
  case pfs::task_state::running:
    return "Running";
  case pfs::task_state::sleeping:
    return "Sleeping";
  case pfs::task_state::disk_sleep:
    return "Disk Sleep";
  case pfs::task_state::stopped:
    return "Stopped";
  case pfs::task_state::tracing_stop:
    return "Tracing Stop";
  case pfs::task_state::zombie:
    return "Zombie";
  case pfs::task_state::dead:
    return "Dead";
  case pfs::task_state::wakekill:
    return "Wakekill";
  case pfs::task_state::waking:
    return "Waking";
  case pfs::task_state::parked:
    return "Parked";
  case pfs::task_state::idle:
    return "Idle";
  }
}

std::string get_username_from_uid(int uid) {
  struct passwd *pw = getpwuid(uid);
  if (pw) {
    return {pw->pw_name};
  }
  return {};
}

OperativeSystem::OperativeSystem() {
  // Get kernel information.
  Kernel kernel;
  struct utsname info{};
  uname(&info);

  kernel.sysname = info.sysname;
  kernel.nodename = info.nodename;
  kernel.release = info.release;
  kernel.version = info.version;
  kernel.machine = info.machine;

  _kernel = kernel;

  snap();
}

void OperativeSystem::snap() {
  struct sysinfo system_info{};
  auto processes = pfs::procfs().get_processes();

  // Clean map of processes.
  _processes.clear();

  for (auto& process : processes) {
    sysinfo(&system_info);
    Process temporary_process;
    temporary_process.id = process.id();
    temporary_process.user = get_username_from_uid(process.get_status().uid.effective);
    temporary_process.status = parse_state_type(process.get_status().state);
    //temporary_process.time = std::chrono::duration<long, std::micro>(std::chrono::seconds(system_info.uptime) - std::chrono::seconds(process.get_stat().starttime/CLOCKS_PER_SEC));
    auto command_line = process.get_cmdline();
    temporary_process.command = std::accumulate(command_line.begin(), command_line.end(), std::string{});
    temporary_process.threads = process.get_status().threads;

    _processes[temporary_process.id] = temporary_process;
  }
  // Get uptime.
  sysinfo(&system_info);
  _uptime = std::chrono::seconds(system_info.uptime);

  // Get CPU load.
  _load1m = system_info.loads[0]*LOAD;
  _load5m = system_info.loads[1]*LOAD;
  _load15m = system_info.loads[2]*LOAD;
}
const Kernel &OperativeSystem::kernel() const { return _kernel; }

const std::chrono::seconds &OperativeSystem::uptime() const {
  return _uptime;
}

double OperativeSystem::load_1m() const { return _load1m; }

double OperativeSystem::load_5m() const { return _load5m; }

double OperativeSystem::load_15m() const { return _load15m; }

const std::map<unsigned int, Process> &OperativeSystem::processes() const {
  return _processes;
}
