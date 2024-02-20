#include "data-sources/latency/standards.h"

std::string TAS::type_of_operation_name(const TypeOfOperation& type) {
    switch (type) {
        case TypeOfOperation::SetGateStates:
            return "set";
        case TypeOfOperation::SetAndHoldMac:
            return "set and hold";
        case TypeOfOperation::SetAndReleaseMac:
            return "set and release";
    }
}

rapidjson::Document TAS::SchedParameters::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    rapidjson::Value value;

    // Handle get_parameter_table.
    if (gate_parameter_table.has_value()) {
        value.SetArray();
        json_document.AddMember("gate-parameter-table", value, allocator);
        for (const auto& table_entry : gate_parameter_table.value()) {
            value.SetObject();

            rapidjson::Value subvalue;

            subvalue.SetUint(table_entry.traffic_class_type);
            value.AddMember("traffic-class-type", subvalue, allocator);
            if (table_entry.queue_max_sdu.has_value()) {
                subvalue.SetUint(table_entry.queue_max_sdu.value());
            } else {
                subvalue.SetObject();
            }
            value.AddMember("queue-max-sdu", subvalue, allocator);
            if (table_entry.transmission_overrun.has_value()) {
                subvalue.SetUint(table_entry.transmission_overrun.value());
            } else {
                subvalue.SetObject();
            }
            value.AddMember("transmission-overrun", subvalue, allocator);

            json_document["gate-parameter-table"].PushBack(value, allocator);
        }
    } else {
        value.SetObject();
        json_document.AddMember("gate-parameter-table", value, allocator);
    }

    // Handle gate_enabled.
    if (gate_enabled.has_value()) {
        value.SetBool(gate_enabled.value());
        json_document.AddMember("gate-enabled", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("gate-enabled", value, allocator);
    }

    // Handle admin_gate_states.
    if (admin_gate_states.has_value()) {
        value.SetUint(admin_gate_states.value());
        json_document.AddMember("admin-gate-states", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("admin-gate-states", value, allocator);
    }

    // Handle oper_gate_states.
    if (oper_gate_states.has_value()) {
        value.SetUint(oper_gate_states.value());
        json_document.AddMember("oper-gate-states", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("oper-gate-states", value, allocator);
    }

    // Handle admin_control_list.
    if (admin_control_list.has_value()) {
        value.SetArray();
        json_document.AddMember("admin-control-list", value, allocator);
        for (const auto& entry: admin_control_list.value()) {
            value.SetObject();

            rapidjson::Value subvalue;

            subvalue.SetUint(entry.index);
            value.AddMember("index", subvalue, allocator);
            subvalue.SetString(rapidjson::GenericStringRef(type_of_operation_name(entry.operation_name).c_str()));
            value.AddMember("operation-name", subvalue, allocator);
            if (entry.time_interval_value.has_value()) {
                subvalue.SetUint(entry.time_interval_value.value());
            } else {
                subvalue.SetObject();
            }
            value.AddMember("time-interval-value", subvalue, allocator);
            subvalue.SetUint(entry.gate_states_value);
            value.AddMember("gate-states-value", subvalue, allocator);

            json_document["admin-control-list"].PushBack(value, allocator);
        }
    } else {
        value.SetObject();
        json_document.AddMember("admin-control-list", value, allocator);
    }

    // Handle oper_control_list.
    if (oper_control_list.has_value()) {
        value.SetArray();
        json_document.AddMember("oper-control-list", value, allocator);
        for (const auto& entry: oper_control_list.value()) {
            value.SetObject();

            rapidjson::Value subvalue;

            subvalue.SetUint(entry.index);
            value.AddMember("index", subvalue, allocator);
            subvalue.SetString(rapidjson::GenericStringRef(type_of_operation_name(entry.operation_name).c_str()));
            value.AddMember("operation-name", subvalue, allocator);
            if (entry.time_interval_value.has_value()) {
                subvalue.SetUint(entry.time_interval_value.value());
            } else {
                subvalue.SetObject();
            }
            value.AddMember("time-interval-value", subvalue, allocator);
            subvalue.SetUint(entry.gate_states_value);
            value.AddMember("gate-states-value", subvalue, allocator);

            json_document["oper-control-list"].PushBack(value, allocator);
        }
    } else {
        value.SetObject();
        json_document.AddMember("oper-control-list", value, allocator);
    }

    // Handle admin_cycle_time.
    if (admin_cycle_time.has_value()) {
        value.SetObject();

        rapidjson::Value subvalue;

        subvalue.SetUint(admin_cycle_time.value().numerator);
        value.AddMember("numerator", subvalue, allocator);
        subvalue.SetUint(admin_cycle_time.value().denominator);
        value.AddMember("denominator", subvalue, allocator);

        json_document.AddMember("admin-cycle-time", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("admin-cycle-time", value, allocator);
    }

    // Handle oper_cycle_time.
    if (oper_cycle_time.has_value()) {
        value.SetObject();

        rapidjson::Value subvalue;

        subvalue.SetUint(oper_cycle_time.value().numerator);
        value.AddMember("numerator", subvalue, allocator);
        subvalue.SetUint(oper_cycle_time.value().denominator);
        value.AddMember("denominator", subvalue, allocator);

        json_document.AddMember("oper-cycle-time", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("oper-cycle-time", value, allocator);
    }

    // Handle admin_cycle_time_extension.
    if (admin_cycle_time_extension.has_value()) {
        value.SetUint(admin_cycle_time_extension.value());
        json_document.AddMember("admin-cycle-time-extension", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("admin-cycle-time-extension", value, allocator);
    }

    // Handle oper_cycle_time_extension.
    if (oper_cycle_time_extension.has_value()) {
        value.SetUint(oper_cycle_time_extension.value());
        json_document.AddMember("oper-cycle-time-extension", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("oper-cycle-time-extension", value, allocator);
    }

    // Handle admin_base_time.
    if (admin_base_time.has_value()) {
        value.SetObject();

        rapidjson::Value subvalue;

        subvalue.SetUint64(admin_base_time.value().seconds);
        value.AddMember("seconds", subvalue, allocator);
        subvalue.SetUint(admin_base_time.value().nanoseconds);
        value.AddMember("nanoseconds", subvalue, allocator);

        json_document.AddMember("admin-base-time", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("admin-base-time", value, allocator);
    }

    // Handle oper_base_time.
    if (oper_base_time.has_value()) {
        value.SetObject();

        rapidjson::Value subvalue;

        subvalue.SetUint64(oper_base_time.value().seconds);
        value.AddMember("seconds", subvalue, allocator);
        subvalue.SetUint(oper_base_time.value().nanoseconds);
        value.AddMember("nanoseconds", subvalue, allocator);

        json_document.AddMember("oper-base-time", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("oper-base-time", value, allocator);
    }

    // Handle config_change.
    if (config_change.has_value()) {
        value.SetBool(config_change.value());
        json_document.AddMember("config-changed", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("config-changed", value, allocator);
    }

    // Handle config_change_time.
    if (config_change_time.has_value()) {
        value.SetObject();

        rapidjson::Value subvalue;

        subvalue.SetUint64(config_change_time.value().seconds);
        value.AddMember("seconds", subvalue, allocator);
        subvalue.SetUint(config_change_time.value().nanoseconds);
        value.AddMember("nanoseconds", subvalue, allocator);

        json_document.AddMember("config-change-time", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("config-change-time", value, allocator);
    }

    // Handle tick_granularity.
    if (tick_granularity.has_value()) {
        value.SetUint(tick_granularity.value());
        json_document.AddMember("tick-granularity", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("tick-granularity", value, allocator);
    }

    // Handle currentTime.
    if (currentTime.has_value()) {
        value.SetObject();

        rapidjson::Value subvalue;

        subvalue.SetUint64(currentTime.value().seconds);
        value.AddMember("seconds", subvalue, allocator);
        subvalue.SetUint(currentTime.value().nanoseconds);
        value.AddMember("nanoseconds", subvalue, allocator);

        json_document.AddMember("current-time", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("current-time", value, allocator);
    }

    // Handle config_pending.
    if (config_pending.has_value()) {
        value.SetUint(config_pending.value());
        json_document.AddMember("config-pending", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("config-pending", value, allocator);
    }

    // Handle config_change_error.
    if (config_change_error.has_value()) {
        value.SetUint64(config_change_error.value());
        json_document.AddMember("config-change-error", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("config-change-error", value, allocator);
    }

    // Handle supported_list_max.
    if (supported_list_max.has_value()) {
        value.SetUint(supported_list_max.value());
        json_document.AddMember("supported-list-max", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("supported-list-max", value, allocator);
    }

    // Handle supported_cycle_max.
    if (supported_cycle_max.has_value()) {
        value.SetObject();

        rapidjson::Value subvalue;

        subvalue.SetUint(supported_cycle_max.value().numerator);
        value.AddMember("numerator", subvalue, allocator);
        subvalue.SetUint(supported_cycle_max.value().denominator);
        value.AddMember("denominator", subvalue, allocator);

        json_document.AddMember("supported_cycle_max", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("supported_cycle_max", value, allocator);
    }

    // Handle supported_interval_max.
    if (supported_interval_max.has_value()) {
        value.SetUint(supported_interval_max.value());
        json_document.AddMember("supported-interval-max", value, allocator);
    } else {
        value.SetObject();
        json_document.AddMember("supported-interval-max", value, allocator);
    }


    return json_document;
}

rapidjson::Document CBS::CBSAConfig::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    rapidjson::Value value;

    if (cbsa_parameters.has_value()) {
        value.SetArray();
        json_document.AddMember("parameters", value, allocator);
        for (const auto& rule: cbsa_parameters.value()) {
            value.SetObject();

            rapidjson::Value subvalue;

            subvalue.SetUint(rule.traffic_class);
            value.AddMember("traffic-class", subvalue, allocator);
            if (rule.admin_idle_slope.has_value()) {
                subvalue.SetUint64(rule.admin_idle_slope.value());
            } else {
                subvalue.SetObject();
            }
            value.AddMember("admin-idle-slope", subvalue, allocator);
            if (rule.oper_idle_slope.has_value()) {
                subvalue.SetUint64(rule.oper_idle_slope.value());
            } else {
                subvalue.SetObject();
            }
            value.AddMember("oper-idle-slope", subvalue, allocator);

            json_document["parameters"].PushBack(value, allocator);
        }
    }

    return json_document;
}