#include "data-sources/os.h"

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
    struct passwd* pw = getpwuid(uid);
    if (pw) {
        return {pw->pw_name};
    }
    return {};
}

OperativeSystem::OperativeSystem() {
    // Get kernel information.
    Kernel kernel;
    struct utsname info {};
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
    struct sysinfo system_info {};
    auto processes = pfs::procfs().get_processes();

    // Clean map of processes.
    _processes.clear();

    for (auto& process : processes) {
        sysinfo(&system_info);
        Process temporary_process;
        temporary_process.id = process.id();
        temporary_process.user = get_username_from_uid(process.get_status().uid.effective);
        temporary_process.status = parse_state_type(process.get_status().state);
        // temporary_process.time = std::chrono::duration<long, std::micro>(std::chrono::seconds(system_info.uptime) -
        // std::chrono::seconds(process.get_stat().starttime/CLOCKS_PER_SEC));
        auto command_line_vector = process.get_cmdline();
        std::string command_line;
        for (const auto& item : command_line_vector) {
            command_line += item + " ";
        }
        command_line.pop_back();
        temporary_process.command = command_line.c_str();
        temporary_process.threads = process.get_status().threads;

        _processes[temporary_process.id] = temporary_process;
    }
    // Get uptime.
    sysinfo(&system_info);
    _uptime = std::chrono::seconds(system_info.uptime);

    // Get CPU load.
    _load1m = system_info.loads[0] * LOAD;
    _load5m = system_info.loads[1] * LOAD;
    _load15m = system_info.loads[2] * LOAD;
}
const Kernel& OperativeSystem::kernel() const {
    return _kernel;
}

const std::chrono::seconds& OperativeSystem::uptime() const {
    return _uptime;
}

double OperativeSystem::load_1m() const {
    return _load1m;
}

double OperativeSystem::load_5m() const {
    return _load5m;
}

double OperativeSystem::load_15m() const {
    return _load15m;
}

const std::map<unsigned int, Process>& OperativeSystem::processes() const {
    return _processes;
}

void OperativeSystem::enable_json_output() {
    _json_formatted_output = true;
}

void OperativeSystem::disable_json_output() {
    _json_formatted_output = false;
}

rapidjson::Document OperativeSystem::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value.
    rapidjson::Value value;

    value.SetObject();
    json_document.AddMember("kernel", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_kernel.sysname.c_str()));
    json_document["kernel"].AddMember("sysname", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_kernel.nodename.c_str()));
    json_document["kernel"].AddMember("nodename", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_kernel.release.c_str()));
    json_document["kernel"].AddMember("release", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_kernel.version.c_str()));
    json_document["kernel"].AddMember("version", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_kernel.machine.c_str()));
    json_document["kernel"].AddMember("machine", value, allocator);
    value.SetInt64(_uptime.count());
    json_document.AddMember("uptime", value, allocator);
    value.SetDouble(_load1m);
    json_document.AddMember("load-1m", value, allocator);
    value.SetDouble(_load5m);
    json_document.AddMember("load-5m", value, allocator);
    value.SetDouble(_load15m);
    json_document.AddMember("load-15m", value, allocator);

    value.SetObject();
    json_document.AddMember("processes", value, allocator);
    for (const auto& [pid, process] : _processes) {
        std::string pid_string = std::to_string(pid);
        value.SetObject();
        rapidjson::Value name;
        name.SetString(pid_string.c_str(), pid_string.size(), allocator);
        json_document["processes"].AddMember(name, value, allocator);
        value.SetString(rapidjson::GenericStringRef(process.user.c_str()));
        json_document["processes"][pid_string.c_str()].AddMember("user", value, allocator);
        value.SetDouble(process.cpu_usage);
        json_document["processes"][pid_string.c_str()].AddMember("cpu-usage", value, allocator);
        value.SetDouble(process.memory_usage);
        json_document["processes"][pid_string.c_str()].AddMember("memory-usage", value, allocator);

        switch (process.status) {
            case ProcessStatus::Running:
                value.SetString("running");
                break;
            case ProcessStatus::Sleeping:
                value.SetString("sleeping");
                break;
            case ProcessStatus::DiskSleep:
                value.SetString("disk sleep");
                break;
            case ProcessStatus::Stopped:
                value.SetString("stopped");
                break;
            case ProcessStatus::TracingStop:
                value.SetString("tracing stop");
                break;
            case ProcessStatus::Zombie:
                value.SetString("zombie");
                break;
            case ProcessStatus::Dead:
                value.SetString("dead");
                break;
            case ProcessStatus::Wakekill:
                value.SetString("wakekill");
                break;
            case ProcessStatus::Waking:
                value.SetString("waking");
                break;
            case ProcessStatus::Parked:
                value.SetString("parked");
                break;
            case ProcessStatus::Idle:
                value.SetString("idle");
                break;
        }
        json_document["processes"][pid_string.c_str()].AddMember("status", value, allocator);

        value.SetInt64(process.time.count());
        json_document["processes"][pid_string.c_str()].AddMember("time", value, allocator);
        value.SetString(rapidjson::GenericStringRef(process.command.c_str()));
        json_document["processes"][pid_string.c_str()].AddMember("command", value, allocator);
        value.SetInt(process.threads);
        json_document["processes"][pid_string.c_str()].AddMember("thread", value, allocator);
    }

    return json_document;
}

std::ostream& operator<<(std::ostream& os, const OperativeSystem& operative_system) {
    if (operative_system._json_formatted_output) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        auto json_document = operative_system.json();
        json_document.Accept(writer);

        os << buffer.GetString();
    } else {
        os << "Kernel Information: " << "\n";
        os << "\t" << "System name: " << operative_system._kernel.sysname << "\n";
        os << "\t" << "Node name: " << operative_system._kernel.nodename << "\n";
        os << "\t" << "Release: " << operative_system._kernel.release << "\n";
        os << "\t" << "Version: " << operative_system._kernel.version << "\n";
        os << "\t" << "Machine: " << operative_system._kernel.machine << "\n";
        os << "Uptime and loads:" << "\n";
        os << "\t" << "Uptime: " << operative_system._uptime.count() << " seconds" << "\n";
        os << "\t" << "Load (1m): " << operative_system._load1m << "\n";
        os << "\t" << "Load (5m): " << operative_system._load5m << "\n";
        os << "\t" << "Load (15m): " << operative_system._load15m << "\n";
        os << "Processes: " << "(" << operative_system._processes.size() << ")" << "\n";
        for (const auto& [pid, process] : operative_system._processes) {
            os << "\t" << std::setw(8) << process.user << " " << std::setw(7) << pid << " " << std::setw(5) << std::setprecision(4) << process.cpu_usage << " " << std::setw(5) << std::setprecision(4) << process.memory_usage << " " << process.command << "\n";
        }
        os << std::endl;
    }

    return os;
}
