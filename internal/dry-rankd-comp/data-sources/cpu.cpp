#include "data-sources/cpu.h"

static unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::mt19937 range(seed);
static std::uniform_int_distribution<std::mt19937::result_type> full_randomizer(0, RAND_MAX);

CPU::CPU() {
    // Initialize settings of CPU.
    _byte_order = LITTLE;
    _vendor_id = "Generic vendor ID";
    _family = "Generic CPU family";
    _model = "CPU for Simuzilla, version 1";
    _model_name = "SimuCPU";
    _stepping = "";
    _microcode = "";
    _flags = {"A", "B"};
    _bugs = {"b1", "b2"};
    _bogomips = 7199.99;

    // Randomize a number of cores.
    std::random_device random_device;
    std::mt19937 rng(random_device());
    std::uniform_int_distribution<std::mt19937::result_type> randomize(0, 4);

    const std::vector<int> core_options = {4, 8, 16, 32, 64};
    const std::vector<int> frequency_options = {2500, 2800, 3000, 3200, 3500};

    int cores = core_options[randomize(rng)];
    int cpu_frequency = frequency_options[randomize(rng)];
    for (int core_index = 0; core_index != cores; core_index++) {
        _cores[core_index] = CPUCore(core_index, cpu_frequency, "8", true);
    }

    // Initialize values for CPUStats.
    _snapshot.system = full_randomizer(range) % 5000 + 1000;
    _snapshot.user = full_randomizer(range) % 10000 + 5000;
    _snapshot.nice = full_randomizer(range) % 1000 + 200;
    _snapshot.irqs = full_randomizer(range) % 300 + 50;
    _snapshot.softirqs = full_randomizer(range) % 200 + 20;
    _snapshot.iowait = full_randomizer(range) % 500 + 100;
    _snapshot.idle = full_randomizer(range) % 20000 + 5000;
    _snapshot.steal = full_randomizer(range) % 100;

    unsigned long long total_time = _snapshot.system + _snapshot.user + _snapshot.nice + _snapshot.irqs + _snapshot.softirqs + _snapshot.iowait + _snapshot.idle + _snapshot.steal;
    _snapshot.in_use = (double)(total_time - _snapshot.idle) / total_time * 100.0;

    _snapshot.total = 100.0;

    // Take a snapshot of the system, CPU-wise.
    snap();
}

uint8_t CPU::identifier() const {
    return _identifier;
}

ByteOrder CPU::byte_order() const {
    return _byte_order;
}

std::string CPU::vendor_id() const {
    return _vendor_id ? _vendor_id.value() : "";
}

std::string CPU::family() const {
    return _family ? _family.value() : "";
}

std::string CPU::model() const {
    return _model ? _model.value() : "";
}

std::string CPU::model_name() const {
    return _model_name ? _model_name.value() : "";
}

std::string CPU::stepping() const {
    return _stepping ? _stepping.value() : "";
}

std::string CPU::microcode() const {
    return _microcode ? _microcode.value() : "";
}

const std::set<std::string>& CPU::flags() const {
    return _flags;
}

const std::set<std::string>& CPU::bugs() const {
    return _bugs;
}

double CPU::bogomips() const {
    return _bogomips;
}

const std::map<int, CPUCore>& CPU::cores() const {
    return _cores;
}

const CPUStats& CPU::snapshot() const {
    return _snapshot;
}

void CPU::snap() {
    // Update all the values for a snap.
    _snapshot.system += (full_randomizer(range) % 100) - 50;
    _snapshot.user += (full_randomizer(range) % 200) - 100;
    _snapshot.nice += (full_randomizer(range) % 50) - 25;
    _snapshot.irqs += (full_randomizer(range) % 20) - 10;
    _snapshot.softirqs += (full_randomizer(range) % 10) - 5;
    _snapshot.iowait += (full_randomizer(range) % 30) - 15;
    _snapshot.idle += (full_randomizer(range) % 200) - 100;
    _snapshot.steal += (full_randomizer(range) % 10);

    if (_snapshot.system < 0) _snapshot.system = 0;
    if (_snapshot.user < 0) _snapshot.user = 0;
    if (_snapshot.nice < 0) _snapshot.nice = 0;
    if (_snapshot.irqs < 0) _snapshot.irqs = 0;
    if (_snapshot.softirqs < 0) _snapshot.softirqs = 0;
    if (_snapshot.iowait < 0) _snapshot.iowait = 0;
    if (_snapshot.idle < 0) _snapshot.idle = 0;
    if (_snapshot.steal < 0) _snapshot.steal = 0;

    unsigned long long total_time = _snapshot.system + _snapshot.user + _snapshot.nice + _snapshot.irqs + _snapshot.softirqs + _snapshot.iowait + _snapshot.idle + _snapshot.steal;
    _snapshot.in_use = (double)(total_time - _snapshot.idle) / total_time * 100.0;

    _snapshot.total = 100.0;
}
