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
        int unit = 0;
        unit_parsing_tree.clear();

        while (true) {
            // Get title of attribute. If none, get out, since this must the EOB.
            getline(cpuinfo, line, ':');
            if (cpuinfo.eof()) {
                break;
            }
            std::string key = trim(line);
            unit_parsing_tree[key] = "";

            // Add the value to the given key in the map.
            getline(cpuinfo, unit_parsing_tree[key]);
        }

        // Add the register onto the main parsing tree.
        parsing_tree[unit++] = unit_parsing_tree;
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

rapidjson::Document CPU::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    json_document["id"].SetInt(_identifier);
    if (_byte_order == LITTLE) {
        json_document["endianess"].SetString("little endian");
    } else {
        json_document["endianess"].SetString("big endian");
    }
    json_document["vendor"].SetString(rapidjson::GenericStringRef(_vendor_id.value_or("").c_str()));
    json_document["family"].SetString(rapidjson::GenericStringRef(_family.value_or("").c_str()));
    json_document["model"].SetString(rapidjson::GenericStringRef(_model.value_or("").c_str()));
    json_document["model-name"].SetString(rapidjson::GenericStringRef(_model_name.value_or("").c_str()));
    json_document["stepping"].SetString(rapidjson::GenericStringRef(_stepping.value_or("").c_str()));
    json_document["microcode"].SetString(rapidjson::GenericStringRef(_microcode.value_or("").c_str()));

    std::string flags;
    for (const auto& flag : _flags) {
        flags += flag + " ";
    }
    json_document["flags"].SetString(rapidjson::GenericStringRef(flags.c_str()));

    std::string bugs;
    for (const auto& bug : _bugs) {
        bugs += bug + " ";
    }
    json_document["bugs"].SetString(rapidjson::GenericStringRef(bugs.c_str()));

    json_document["bogomips"].SetDouble(_bogomips);
    json_document["cores"].SetObject();
    for (const auto& [core_id, core] : _cores) {
        json_document["cores"][core_id].SetObject();
        json_document["cores"][core_id]["frequency"].SetDouble(core.frequency.value_or(NAN));
        json_document["cores"][core_id]["cache-size"].SetString(rapidjson::GenericStringRef(core.cache_size.value_or("").c_str()));
        json_document["cores"][core_id]["has-fpu"].SetBool(core.has_fpu.value_or(false));
    }

    json_document["stats"].SetObject();
    json_document["stats"]["system"].SetUint64(_snapshot.system);
    json_document["stats"]["user"].SetUint64(_snapshot.user);
    json_document["stats"]["nice"].SetUint64(_snapshot.nice);
    json_document["stats"]["irqs"].SetUint64(_snapshot.irqs);
    json_document["stats"]["softirqs"].SetUint64(_snapshot.softirqs);
    json_document["stats"]["iowait"].SetUint64(_snapshot.iowait);
    json_document["stats"]["idle"].SetUint64(_snapshot.idle);
    json_document["stats"]["steal"].SetUint64(_snapshot.steal);
    json_document["stats"]["in-use"].SetDouble(_snapshot.in_use);
    json_document["stats"]["total"].SetDouble(_snapshot.total);

    return json_document;
}

std::ostream& operator<<(std::ostream& os, const CPU& cpu) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    auto json_document = cpu.json();
    json_document.Accept(writer);

    os << buffer.GetString();

    return os;
}
