#ifndef RANKDTIME_LATENCY_STANDARDS_H
#define RANKDTIME_LATENCY_STANDARDS_H

#include <cstdint>
#include <optional>
#include <vector>

#include "standards/ieee.h"

namespace CBS {
    struct CBSAParametersTableEntry {
        uint8_t traffic_class;
        std::optional<uint64_t> admin_idle_slope = 0;
        std::optional<uint64_t> oper_idle_slope = 0;
    };

    typedef std::vector<CBSAParametersTableEntry> CBSAParameters;

    struct CBSAConfig {
        std::optional<CBSAParameters> cbsa_parameters;
    };

    typedef CBSAConfig CBS;
}

namespace TAS {
    typedef ieee802::RationalGrouping SupportedCycleMax;
    typedef ieee802::PTPTimeGrouping CurrentTime;
    typedef ieee802::PTPTimeGrouping ConfigChangeTime;
    typedef ieee802::PTPTimeGrouping AdminBaseTime;
    typedef ieee802::PTPTimeGrouping OperBaseTime;
    typedef ieee802::RationalGrouping AdminCycleTime;
    typedef ieee802::RationalGrouping OperCycleTime;

    enum class TypeOfOperation {
        SetGateStates = 0,
        SetAndHoldMac,
        SetAndReleaseMac
    };

    struct SchedGateControlEntriesEntry {
        uint32_t index;
        TypeOfOperation operation_name;
        std::optional<uint32_t> time_interval_value;
        uint8_t gate_states_value;
    };

    typedef std::vector<SchedGateControlEntriesEntry> SchedGateControlEntries;
    typedef SchedGateControlEntries AdminControlList;
    typedef SchedGateControlEntries OperControlList;

    typedef uint8_t TrafficClassType;

    struct GateParameterTableEntry {
        TrafficClassType traffic_class_type;
        std::optional<uint32_t> queue_max_sdu = 0;
        std::optional<uint64_t> transmission_overrun = 0;
    };

    typedef std::optional<std::vector<GateParameterTableEntry>> GateParameterTable;

    struct SchedParameters {
        std::optional<GateParameterTable> gate_parameter_table;
        std::optional<bool> gate_enabled = false;
        std::optional<uint8_t> admin_gate_states = 255;
        std::optional<uint8_t> oper_gate_states;
        std::optional<AdminControlList> admin_control_list;
        std::optional<OperControlList> oper_control_list;
        std::optional<AdminCycleTime> admin_cycle_time;
        std::optional<OperCycleTime> oper_cycle_time;
        std::optional<uint32_t> admin_cycle_time_extension;
        std::optional<uint32_t> oper_cycle_time_extension;
        std::optional<AdminBaseTime> admin_base_time;
        std::optional<OperBaseTime> oper_base_time;
        std::optional<bool> config_change;
        std::optional<ConfigChangeTime> config_change_time;
        std::optional<uint32_t> tick_granularity;
        std::optional<CurrentTime> currentTime;
        std::optional<bool> config_pending;
        std::optional<uint64_t> config_change_error;
        std::optional<uint32_t> supported_list_max;
        std::optional<SupportedCycleMax> supported_cycle_max;
        std::optional<uint32_t> supported_interval_max;
    };

    typedef SchedParameters TAS;
};

#endif //RANKDTIME_LATENCY_STANDARDS_H
