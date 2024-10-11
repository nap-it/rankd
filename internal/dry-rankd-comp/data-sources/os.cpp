#include "data-sources/os.h"

static unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::mt19937 range(seed);
static std::uniform_int_distribution<std::mt19937::result_type> full_randomizer(0, RAND_MAX);

#include <iostream>

OperativeSystem::OperativeSystem() {
    // Initialize kernel information.
    _kernel.sysname = "Operative System SimulOS";
    _kernel.nodename = "Node name SimuNode";
    _kernel.release = "Africa";
    _kernel.version = "1.0.0";
    _kernel.machine = "Mac1";

    // Initialize uptime.
    _starting_time_point = std::chrono::system_clock::now().time_since_epoch().count();
    _uptime = std::chrono::seconds(0);

    // Initialize loads at 1 minute, 5 minutes, and 15 minutes.
    _load1m = (full_randomizer(range) % 1000) / 100.0;
    _load5m = (full_randomizer(range) % 800) / 100.0;
    _load15m = (full_randomizer(range) % 500) / 100.0;

    // Initialize a list of processes.
    int number_of_processes = (full_randomizer(range) % 10) + 10;
    for (int process_index = 0; process_index != number_of_processes; process_index++) {
        Process process;
        process.id = process_index;
        process.user = random_user();
        process.cpu_usage = (full_randomizer(range) % 1000) / 10.0;
        process.memory_usage = (full_randomizer(range) % 10000) / 10.0;
        process.status = random_status();
        process.time = std::chrono::microseconds(full_randomizer(range) % 1000000000);
        process.command = random_command();
        process.threads = full_randomizer(range) % 16 + 1;

        _processes[process_index] = process;
    }

    // Take a snapshot of the system.
    snap();
}

void OperativeSystem::snap() {
    // Update all the variables as in a snap.
    _uptime = std::chrono::seconds(std::chrono::seconds(static_cast<long long>(std::chrono::system_clock::now().time_since_epoch().count() / 1e9) - static_cast<long long>(_starting_time_point))
    );

    _load1m += (full_randomizer(range) % 100 - 50) / 100.0;
    _load5m += (full_randomizer(range) % 80 - 40) / 100.0;
    _load15m += (full_randomizer(range) % 50 - 25) / 100.0;

    if (_load1m < 0) _load1m = 0;
    if (_load5m < 0) _load5m = 0;
    if (_load15m < 0) _load15m = 0;

    for (auto& [pid, process]: _processes) {
        process.cpu_usage += ((full_randomizer(range) % 200) - 100) / 100.0;
        if (process.cpu_usage < 0) process.cpu_usage = 0;
        if (process.cpu_usage > 100) process.cpu_usage = 100;

        process.memory_usage += ((full_randomizer(range) % 200) - 100) / 10.0;
        if (process.memory_usage < 0) process.memory_usage = 0;

        if (full_randomizer(range) % 10 == 0) {
            process.status = random_status();
        }

        process.time += std::chrono::microseconds(full_randomizer(range) % 1000000);
    }
}
const Kernel& OperativeSystem::kernel() const {
    return _kernel;
}

const std::chrono::seconds& OperativeSystem::uptime() const {
    return _uptime;
}

double OperativeSystem::load_1m() const {
    return (_uptime >= std::chrono::minutes(1)) ? _load1m : 0.0;
}

double OperativeSystem::load_5m() const {
    return (_uptime >= std::chrono::minutes(5)) ? _load5m : 0.0;
}

double OperativeSystem::load_15m() const {
    return (_uptime >= std::chrono::minutes(15)) ? _load15m : 0.0;
}

const std::map<unsigned int, Process>& OperativeSystem::processes() const {
    return _processes;
}

ProcessStatus random_status() {
    int status = full_randomizer(range) % 11; // 11 possible statuses
    return static_cast<ProcessStatus>(status);
}

std::string random_user() {
    std::vector<std::string> users = {"root", "alice", "bob", "carol", "dave"};
    return users[full_randomizer(range) % users.size()];
}

std::string random_command() {
    std::vector<std::string> commands = {"/bin/bash", "python3 script.py", "java MyApp", "nginx", "systemd", "ls", "dmesg", "cargo build", "sl", "asciiquarium"};
    return commands[full_randomizer(range) % commands.size()];
}