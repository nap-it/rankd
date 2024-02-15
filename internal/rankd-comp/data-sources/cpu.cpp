#include "data-sources/cpu.h"

#include <iostream>

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
        int unit = -1;

        while (true) {
            // Get title of attribute. If none, get out, since this must the EOB.
            getline(cpuinfo, line, ':');
            if (cpuinfo.eof()) {
                break;
            }
            std::string key = trim(line);
            if (key == "processor" && unit != -1) {
                // Add the register onto the main parsing tree.
                parsing_tree[unit++] = unit_parsing_tree;
                unit_parsing_tree.clear();
            }
            if (unit == -1) {
                unit++;
            }
            unit_parsing_tree[key] = "";

            // Add the value to the given key in the map.
            cpuinfo.get();
            getline(cpuinfo, unit_parsing_tree[key]);
        }

        parsing_tree[unit] = unit_parsing_tree;
    }
    cpuinfo.close();

    _identifier = 0;       // TODO Get information on this identifier.
    _byte_order = LITTLE;  // TODO Get this information.
    _vendor_id =
            (parsing_tree[0].count("vendor_id") != 0) ? std::optional(parsing_tree[0].at("vendor_id")) : std::nullopt;
    _family =
            (parsing_tree[0].count("cpu family") != 0) ? std::optional(parsing_tree[0].at("cpu family")) : std::nullopt;
    _model =
            (parsing_tree[0].count("model name") != 0) ? std::optional(parsing_tree[0].at("model name")) : std::nullopt;
    _model_name = (parsing_tree[0].count("model") != 0) ? std::optional(parsing_tree[0].at("model")) : std::nullopt;
    _stepping = (parsing_tree[0].count("stepping") != 0) ? std::optional(parsing_tree[0].at("stepping")) : std::nullopt;
    _microcode =
            (parsing_tree[0].count("microcode") != 0) ? std::optional(parsing_tree[0].at("microcode")) : std::nullopt;

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
    _bogomips = (unit_parsing_tree.count("bogomips") != 0) ? std::stod(unit_parsing_tree.at("bogomips"))
                                                           : std::stod(unit_parsing_tree.at("BogoMIPS"));

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

    stats.total = stats.system + stats.user + stats.nice + stats.irqs + stats.softirqs + stats.iowait + stats.idle +
                  stats.steal;
    stats.in_use = (stats.system + stats.user + stats.nice + stats.irqs + stats.softirqs + stats.iowait + stats.steal) /
                   stats.total;

    _snapshot = stats;
}

void CPU::enable_json_output() {
    _json_formatted_output = true;
}

void CPU::disable_json_output() {
    _json_formatted_output = false;
}

rapidjson::Document CPU::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value to retain what is going to the JSON document.
    rapidjson::Value value;

    value.SetInt(_identifier);
    json_document.AddMember("id", value, allocator);

    if (_byte_order == LITTLE) {
        value.SetString("little endian");
    } else {
        value.SetString("big endian");
    }
    json_document.AddMember("endianness", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_vendor_id->c_str()));
    json_document.AddMember("vendor", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_family->c_str()));
    json_document.AddMember("family", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_model->c_str()));
    json_document.AddMember("model", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_model_name->c_str()));
    json_document.AddMember("model-name", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_stepping->c_str()));
    json_document.AddMember("stepping", value, allocator);
    value.SetString(rapidjson::GenericStringRef(_microcode->c_str()));
    json_document.AddMember("microcode", value, allocator);

    std::string flags = "";
    for (const auto& flag : _flags) {
        flags += flag + " ";
    }
    flags.pop_back();
    value.SetString(flags.c_str(), flags.size(), allocator);
    json_document.AddMember("flags", value, allocator);

    std::string bugs = "";
    for (const auto& bug : _bugs) {
        bugs += bug + " ";
    }
    bugs.pop_back();
    value.SetString(bugs.c_str(), bugs.size(), allocator);
    json_document.AddMember("bugs", value, allocator);

    value.SetDouble(_bogomips);
    json_document.AddMember("bogomips", value, allocator);
    value.SetObject();
    json_document.AddMember("cores", value, allocator);
    for (const auto& [core_id, core] : _cores) {
        value.SetObject();
        rapidjson::Value name;
        std::string core_id_string = std::to_string(core_id);
        name.SetString(core_id_string.c_str(), core_id_string.size(), allocator);
        json_document["cores"].AddMember(name, value, allocator);
        value.SetDouble(core.frequency.value_or(NAN));
        json_document["cores"][core_id_string.c_str()].AddMember("frequency", value, allocator);
        std::string cache_size = core.cache_size.value_or("");
        value.SetString(cache_size.c_str(), cache_size.size(), allocator);
        json_document["cores"][core_id_string.c_str()].AddMember("cache-size", value, allocator);
        value.SetBool(core.has_fpu.value_or(false));
        json_document["cores"][core_id_string.c_str()].AddMember("has-fpu", value, allocator);
    }

    value.SetObject();
    json_document.AddMember("stats", value, allocator);
    value.SetUint64(_snapshot.system);
    json_document["stats"].AddMember("system", value, allocator);
    value.SetUint64(_snapshot.user);
    json_document["stats"].AddMember("user", value, allocator);
    value.SetUint64(_snapshot.nice);
    json_document["stats"].AddMember("nice", value, allocator);
    value.SetUint64(_snapshot.irqs);
    json_document["stats"].AddMember("irqs", value, allocator);
    value.SetUint64(_snapshot.softirqs);
    json_document["stats"].AddMember("softirqs", value, allocator);
    value.SetUint64(_snapshot.iowait);
    json_document["stats"].AddMember("iowait", value, allocator);
    value.SetUint64(_snapshot.idle);
    json_document["stats"].AddMember("idle", value, allocator);
    value.SetUint64(_snapshot.steal);
    json_document["stats"].AddMember("steal", value, allocator);
    value.SetDouble(_snapshot.in_use);
    json_document["stats"].AddMember("in-use", value, allocator);
    value.SetDouble(_snapshot.total);
    json_document["stats"].AddMember("total", value, allocator);

    return json_document;
}

std::ostream& operator<<(std::ostream& os, const CPU& cpu) {
    if (cpu._json_formatted_output) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        auto json_document = cpu.json();
        json_document.Accept(writer);

        os << buffer.GetString();
    } else {
        if (cpu._model.has_value()) {
            os << "CPU model: " << cpu._model.value() << "\n";
        }
        os << "Endianess: " << (cpu._byte_order == LITTLE ? "little endian" : "big endian") << "\n";
        if (cpu._stepping.has_value()) {
            os << "Stepping: " << cpu._stepping.value() << "\n";
        }
        if (cpu._microcode.has_value()) {
            os << "Microcode: " << cpu._microcode.value() << "\n";
        }
        os << "Flags: ";
        for (const auto &flag: cpu._flags) {
            os << flag << " ";
        }
        os << "\n";
        os << "Bugs: ";
        for (const auto &bug: cpu._bugs) {
            os << bug << " ";
        }
        os << "\n";
        os << "Bogomips: " << cpu._bogomips << "\n";
        os << "Number of cores: " << cpu._cores.size() << "\n";
        os << "Cores: " << "\n";
        for (const auto &[index, core]: cpu._cores) {
            os << "\t" << "Core number " << index << ": " << "\n";
            if (core.frequency.has_value()) os << "\t  " << "Frequency: " << core.frequency.value() << "\n";
            if (core.cache_size.has_value()) os << "\t  " << "Cache size: " << core.cache_size.value() << "\n";
            if (core.has_fpu.has_value()) os << "\t  " << "Has FPU: " << std::boolalpha << core.has_fpu.value() << "\n";
        }
    }

    return os;
}
