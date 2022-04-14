#include "linux/cpu.h"

CPU::CPU() {
  std::ifstream cpuinfo("/proc/cpuinfo");

  if (not cpuinfo.is_open()) {
    // TODO Throw error.
  }

  // Create parsing temporary variables.
  std::map<std::string, std::string> unit_parsing_tree;
  std::map<int, std::map<std::string, std::string>> parsing_tree;

  // While the file is good to read, read blocks of data.
  while (cpuinfo.good()) {
    std::string line;
    int unit = 0;
    unit_parsing_tree.clear();

    while (true) {
      // Get title of attribute. If none, get out, since this must the EOB.
      getline(cpuinfo, line, ':');
      if (cpuinfo.eof()) {
        break;
      }
      std::string key = line.substr(0, line.find(' '));
      key.erase(std::remove(key.begin(), key.end(), '\n'), key.end());
      key.erase(std::remove(key.begin(), key.end(), '\t'), key.end());
      unit_parsing_tree[key] = "";

      // Add the value to the given key in the map.
      getline(cpuinfo, unit_parsing_tree[key]);
    }

    // Add the register onto the main parsing tree.
    parsing_tree[unit++] = unit_parsing_tree;
  }
  cpuinfo.close();

  _identifier = 0;  // TODO Get information on this identifier.
  _byte_order = LITTLE; // TODO Get this information.
  _vendor_id = (parsing_tree[0].count("vendor_id") != 0) ? std::optional(parsing_tree[0].at("vendor_id")) : std::nullopt;
  _family = (parsing_tree[0].count("cpu family") != 0) ? std::optional(parsing_tree[0].at("cpu family")) : std::nullopt;
  _model = (parsing_tree[0].count("model") != 0) ? std::optional(parsing_tree[0].at("model")) : std::nullopt;
  _model_name = (parsing_tree[0].count("model name") != 0) ? std::optional(parsing_tree[0].at("model name")) : std::nullopt;
  _stepping = (parsing_tree[0].count("stepping") != 0) ? std::optional(parsing_tree[0].at("stepping")) : std::nullopt;
  _microcode = (parsing_tree[0].count("microcode") != 0) ? std::optional(parsing_tree[0].at("microcode")) : std::nullopt;

  // Parse flags.
  std::stringstream flags;
  if (unit_parsing_tree.count("flags")) {
    flags.str(parsing_tree[0].at("flags"));
  } else if (unit_parsing_tree.count("Features")) {
    flags.str(parsing_tree[0].at("Features"));
  } else {
    _flags = std::set<std::string>();
  }

  while (!flags.eof()) {
    std::string item;
    flags >> item;
    _flags.insert(item);
  }

  // Parse bugs.
  std::stringstream bugs;
  if (unit_parsing_tree.count("bugs")) {
    bugs.str(parsing_tree[0].at("bugs"));
    while (!bugs.eof()) {
      std::string item;
      bugs >> item;
      _bugs.insert(item);
    }
  }

  // Parse BogoMIPS.
  _bogomips = (unit_parsing_tree.count("bogomips") != 0) ? std::stod(unit_parsing_tree.at("bogomips")) : std::stod(unit_parsing_tree.at("BogoMIPS"));

  // Parse core pieces of information.
  for (auto [identifier, core] : parsing_tree) {
    CPUCore cpu_core = CPUCore();
    cpu_core.identifier = identifier;
    cpu_core.frequency = core.count("cpu MHz") != 0 ? std::optional(std::stod(core.at("cpu MHz"))) : std::nullopt;
    cpu_core.cache_size = core.count("cache size") != 0 ? std::optional(core.at("cache size")) : std::nullopt;
    cpu_core.has_fpu = core.count("fpu") != 0 ? std::optional(core.at("fpu") == "yes") : std::nullopt;
    _cores[identifier] = cpu_core;
  }

  // Take a snapshot of the system, CPU-wise.
  snap();
}

uint8_t CPU::identifier() const { return _identifier; }

ByteOrder CPU::byte_order() const { return _byte_order; }

const std::optional<std::string> &CPU::vendor_id() const {
  return _vendor_id;
}

const std::optional<std::string> &CPU::family() const { return _family; }

const std::optional<std::string> &CPU::model() const { return _model; }

const std::optional<std::string> &CPU::model_name() const {
  return _model_name;
}

const std::optional<std::string> &CPU::stepping() const {
  return _stepping;
}

const std::optional<std::string> &CPU::microcode() const {
  return _microcode;
}

const std::set<std::string> &CPU::flags() const { return _flags; }

const std::set<std::string> &CPU::bugs() const {
  return _bugs;
}

double CPU::bogomips() const { return _bogomips; }

const std::map<int, CPUCore> &CPU::cores() const { return _cores; }

const CPUStats &CPU::snapshot() const { return _snapshot; }

void CPU::snap() {
  pfs::proc_stat stat = pfs::procfs().get_stat();

  CPUStats stats = CPUStats();
  stats.system = stat.cpus.total.system;
  stats.user = stat.cpus.total.user;
  stats.nice = stat.cpus.total.nice;
  stats.irqs = stat.cpus.total.irq;
  stats.softirqs = stat.cpus.total.softirq;
  stats.iowait = stat.cpus.total.iowait;
  stats.idle = stat.cpus.total.idle;
  stats.steal = stat.cpus.total.steal;

  _snapshot = stats;
}
