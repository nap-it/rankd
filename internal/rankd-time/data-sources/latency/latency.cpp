#include "data-sources/latency/latency.h"

Latency::Latency() {
#ifndef LINUX_TC
    memset(&_local_point, 0, sizeof(_local_point));
    _local_point.nl_family = AF_NETLINK;
    _local_point.nl_pid = 0;

    if (bind(_socket_descriptor, (struct sockaddr *) &_local_point, sizeof(_local_point)) < 0) {
        close(_socket_descriptor);
        throw std::ios_base::failure("Netlink socket could not be bound in Latency.");
    }

    // From this moment on, route-netlink socket is considered ready-to-use.
#endif
}

void Latency::snap() {
#ifndef LINUX_TC
    snap_tas_via_linux_tc();
    snap_cbs_via_linux_tc();
#elif defined(RELYUM)
    snap_tas_via_relyum();
    snap_cbs_via_relyum();
#endif
}

std::map<uint8_t, TAS::TAS *> Latency::tas() {
    return _time_aware_shaping_rules;
}

std::map<uint8_t, CBS::CBS *> Latency::cbs() {
    return _credit_based_shaping_rules;
}

void Latency::enable_json_output() {
    _json_formatted_output = true;
}

void Latency::disable_json_output() {
    _json_formatted_output = false;
}

void Latency::set_output(uint8_t type) {
    _output_type = type;
}

rapidjson::Document Latency::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value.
    rapidjson::Value value;

    if (std::bitset<8>(_output_type).count() > 1) {
        // If the amount of requested standards is more than one...
    } else {
        // If the amount of request standards is just one...
        switch (_output_type) {
            case OUTPUT_LATENCY_TYPE_CBS:
                value.SetArray();
                json_document.AddMember("cbs-rules", value, allocator);
                for (const auto& [index, cbs_pointer]: _credit_based_shaping_rules) {
                    if (cbs_pointer != nullptr) {
                        value.SetObject();

                        rapidjson::Value subvalue;

                        subvalue.SetUint(index);
                        value.AddMember("index", subvalue, allocator);
                        value.AddMember("rule", cbs_pointer->json(), allocator);

                        json_document["cbs-rules"].PushBack(value, allocator);
                    }
                }
                break;
            case OUTPUT_LATENCY_TYPE_TAS:
                value.SetArray();
                json_document.AddMember("tas-rules", value, allocator);
                for (const auto& [index, tas_pointer]: _time_aware_shaping_rules) {
                    if (tas_pointer != nullptr) {
                        value.SetObject();

                        rapidjson::Value subvalue;

                        subvalue.SetUint(index);
                        value.AddMember("index", subvalue, allocator);
                        value.AddMember("rule", tas_pointer->json(), allocator);

                        json_document["tas-rules"].PushBack(value, allocator);
                    }
                }
                break;
            case OUTPUT_LATENCY_TYPE_CQF:
                break;
            case OUTPUT_LATENCY_TYPE_ATS:
                break;
        }
    }

    return json_document;
}

std::ostream &operator<<(std::ostream &os, const Latency &latency) {
    if (latency._json_formatted_output) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        auto json_document = latency.json();
        json_document.Accept(writer);

        os << buffer.GetString();
    } else {
        os << "TSN latency properties and rules:" << "\n";

        os << "\t" << "Credit-based shaper (CBS) rules:" << "\n";
        if (latency._credit_based_shaping_rules.empty()) {
            os << "\t" << "  " << "No rules found on system." << "\n";
        } else {
            for (const auto& [index, rule_pointer] : latency._credit_based_shaping_rules) {
                os << "\t" << "  " << "Rule " << index << ":" << "\n";
                if (rule_pointer == nullptr or (not rule_pointer->cbsa_parameters.has_value())) {
                    os << "\t" << "  " << "  " << "inaccessible" << "\n";
                } else {
                    os << "\t" << "  " << "  " << "Parameters:" << "\n";
                    for (const auto& parameter : rule_pointer->cbsa_parameters.value()) {
                        os << "\t" << "  " << "  " << "  " << "Traffic class: " << parameter.traffic_class << "\n";
                        os << "\t" << "  " << "  " << "  " << "Admin idle slope: ";
                        if (parameter.admin_idle_slope.has_value()) {
                            os << parameter.admin_idle_slope.value() << "\n";
                        } else {
                            os << "undefined" << "\n";
                        }
                        os << "\t" << "  " << "  " << "  " << "Operation idle slope: ";
                        if (parameter.oper_idle_slope.has_value()) {
                            os << parameter.oper_idle_slope.value() << "\n";
                        } else {
                            os << "undefined" << "\n";
                        }
                    }
                }
            }
        }

        os << "\t" << "Time-aware shaper (TAS) rules:" << "\n";
        if (latency._time_aware_shaping_rules.empty()) {
            os << "\t" << "  " << "No rules found on system." << "\n";
        } else {
            for (const auto& [index, rule_pointer] : latency._time_aware_shaping_rules) {
                os << "\t" << "  " << "Rule " << index << ":" << "\n";
                if (rule_pointer == nullptr) {
                    os << "\t" << "  " << "  " << "inaccessible" << "\n";
                } else {
                    os << "\t" << "  " << "  " << "Schedule parameters:" << "\n";
                    os << "\t" << "  " << "  " << "  " << "Gate parameter table: " << "\n";
                    if (rule_pointer->gate_parameter_table.has_value()) {
                        for (const auto& rule : rule_pointer->gate_parameter_table.value()) {
                            os << "\t" << "  " << "  " << "  " << "  " << "Traffic class type: " << rule.traffic_class_type << "\n";
                            if (rule.queue_max_sdu.has_value()) {
                                os << "\t" << "  " << "  " << "  " << "  " << "Queue SDU max: " << rule.queue_max_sdu.value() << "\n";
                            } else {
                                os << "\t" << "  " << "  " << "  " << "  " << "Queue SDU max: " << "undefined" << "\n";
                            }
                            if (rule.transmission_overrun.has_value()) {
                                os << "\t" << "  " << "  " << "  " << "  " << "Transmission overrun: " << rule.transmission_overrun.value() << "\n";
                            } else {
                                os << "\t" << "  " << "  " << "  " << "  " << "Transmission overrun: " << "undefined" << "\n";
                            }
                        }
                    } else {
                        os << "\t" << "  " << "  " << "  " << "  " << "undefined" << "\n";
                    }

                    if (rule_pointer->gate_enabled.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Gate enabled: " << std::boolalpha
                           << rule_pointer->gate_enabled.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Gate enabled: " << "undefined" << "\n";
                    }

                    if (rule_pointer->admin_gate_states.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Admin gate states: " << rule_pointer->admin_gate_states.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Admin gate states: " << "undefined" << "\n";
                    }

                    if (rule_pointer->oper_gate_states.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Oper gate states: " << rule_pointer->oper_gate_states.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Oper gate states: " << "undefined" << "\n";
                    }

                    os << "\t" << "  " << "  " << "  " << "Admin control list: " << "\n";
                    if (rule_pointer->admin_control_list.has_value()) {
                        for (const auto& item : rule_pointer->admin_control_list.value()) {
                            os << "\t" << "  " << "  " << "  " << "  " << "Index: " << item.index << "\n";
                            os << "\t" << "  " << "  " << "  " << "  " << "Operation name: " << TAS::type_of_operation_name(item.operation_name) << "\n";
                            if (item.time_interval_value.has_value()) {
                                os << "\t" << "  " << "  " << "  " << "  " << "Time interval value: " << item.time_interval_value.value() << "\n";
                            } else {
                                os << "\t" << "  " << "  " << "  " << "  " << "Time interval value: " << "undefined" << "\n";
                            }
                            os << "\t" << "  " << "  " << "  " << "  " << "Gate states value: " << item.gate_states_value << "\n";
                        }
                    } else {
                        os << "\t" << "  " << "  " << "  " << "  " << "undefined" << "\n";
                    }

                    os << "\t" << "  " << "  " << "  " << "Admin control list: " << "\n";
                    if (rule_pointer->oper_control_list.has_value()) {
                        for (const auto& item : rule_pointer->oper_control_list.value()) {
                            os << "\t" << "  " << "  " << "  " << "  " << "Index: " << item.index << "\n";
                            os << "\t" << "  " << "  " << "  " << "  " << "Operation name: " << TAS::type_of_operation_name(item.operation_name) << "\n";
                            if (item.time_interval_value.has_value()) {
                                os << "\t" << "  " << "  " << "  " << "  " << "Time interval value: " << item.time_interval_value.value() << "\n";
                            } else {
                                os << "\t" << "  " << "  " << "  " << "  " << "Time interval value: " << "undefined" << "\n";
                            }
                            os << "\t" << "  " << "  " << "  " << "  " << "Gate states value: " << item.gate_states_value << "\n";
                        }
                    } else {
                        os << "\t" << "  " << "  " << "  " << "  " << "undefined" << "\n";
                    }

                    if (rule_pointer->admin_cycle_time.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Admin cycle time: " << (rule_pointer->admin_cycle_time->numerator/rule_pointer->admin_cycle_time->denominator) << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Admin cycle time: " << "undefined" << "\n";
                    }

                    if (rule_pointer->oper_cycle_time.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Oper cycle time: " << (rule_pointer->oper_cycle_time->numerator/rule_pointer->oper_cycle_time->denominator) << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Oper cycle time: " << "undefined" << "\n";
                    }

                    if (rule_pointer->admin_cycle_time_extension.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Admin cycle time extension: " << rule_pointer->admin_cycle_time_extension.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Admin cycle time extension: " << "undefined" << "\n";
                    }

                    if (rule_pointer->oper_cycle_time_extension.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Oper cycle time extension: " << rule_pointer->oper_cycle_time_extension.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Oper cycle time extension: " << "undefined" << "\n";
                    }

                    if (rule_pointer->admin_base_time.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Admin base time: " << rule_pointer->admin_base_time.value().seconds << "s, " << rule_pointer->admin_base_time.value().nanoseconds << "ns" << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Admin base time: " << "undefined" << "\n";
                    }

                    if (rule_pointer->oper_base_time.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Oper base time: " << rule_pointer->oper_base_time.value().seconds << "s, " << rule_pointer->oper_base_time.value().nanoseconds << "ns" << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Oper base time: " << "undefined" << "\n";
                    }

                    if (rule_pointer->config_change.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Config change: " << std::boolalpha
                           << rule_pointer->config_change.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Config change: " << "undefined" << "\n";
                    }

                    if (rule_pointer->config_change_time.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Config change time: " << rule_pointer->config_change_time.value().seconds << "s, " << rule_pointer->config_change_time.value().nanoseconds << "ns" << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Config change time: " << "undefined" << "\n";
                    }

                    if (rule_pointer->tick_granularity.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Tick granularity: " << rule_pointer->tick_granularity.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Tick granularity: " << "undefined" << "\n";
                    }

                    if (rule_pointer->currentTime.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Current time: " << rule_pointer->currentTime.value().seconds << "s, " << rule_pointer->currentTime.value().nanoseconds << "ns" << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Current time: " << "undefined" << "\n";
                    }

                    if (rule_pointer->config_pending.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Config pending: " << std::boolalpha
                           << rule_pointer->config_pending.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Config pending: " << "undefined" << "\n";
                    }

                    if (rule_pointer->config_change_error.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Config change error: " << rule_pointer->config_change_error.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Config change error: " << "undefined" << "\n";
                    }

                    if (rule_pointer->supported_list_max.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Supported list max: " << rule_pointer->supported_list_max.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Supported list max: " << "undefined" << "\n";
                    }

                    if (rule_pointer->supported_cycle_max.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Supported cycle max: " << (rule_pointer->supported_cycle_max->numerator/rule_pointer->supported_cycle_max->denominator) << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Supported cycle max: " << "undefined" << "\n";
                    }

                    if (rule_pointer->supported_interval_max.has_value()) {
                        os << "\t" << "  " << "  " << "  " << "Supported interval max: " << rule_pointer->supported_interval_max.value() << "\n";
                    } else {
                        os << "\t" << "  " << "  " << "  " << "Supported interval max: " << "undefined" << "\n";
                    }
                }
            }
        }
        os << std::endl;
    }

    return os;
}

#ifndef LINUX_TC

void Latency::snap_tas_via_linux_tc() {
    _time_aware_shaping_rules.clear();
    // Declare structure for kernel request on TC.
    struct {
        struct nlmsghdr header;
        struct tcmsg message;
    } request{};

    // Fill up fields for request.
    request.header.nlmsg_len = sizeof(request);
    request.header.nlmsg_type = RTM_GETQDISC;
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    request.header.nlmsg_seq = time(nullptr);
    request.header.nlmsg_pid = 0;
    request.message.tcm_family = AF_UNSPEC;

    // With the request done, just send it.
    if (send(_socket_descriptor, &request, sizeof(request), 0) < 0) {
        close(_socket_descriptor);
        throw std::ios_base::failure("Netlink socket could not be used to send data in Latency-TAS.");
    }

    // Receive the response and handle it accordingly.
    char response[8192];
    ssize_t response_length = recv(_socket_descriptor, response, sizeof(response), 0);
    if (response_length < 0) {
        close(_socket_descriptor);
        throw std::ios_base::failure("Netlink socket could not be used to receive data in Latency-TAS.");
    }

    // Clear all the rules from the main object map.
    _time_aware_shaping_rules.clear();

    // Parse the response fields.
    auto *response_header = (struct nlmsghdr *) response;

    // While there is content to read, check content attributes and, if TAPRIO, properly read data.
    while (NLMSG_OK(response_header, response_length)) {
        if (response_header->nlmsg_type == RTM_NEWQDISC) {
            auto *content = (struct tcmsg *) NLMSG_DATA(response_header);
            auto *attribute = (struct rtattr *) (content + 1);
            int attribute_length = response_header->nlmsg_len - NLMSG_LENGTH(sizeof(*content));
            while (RTA_OK(attribute, attribute_length)) {
                if (attribute->rta_type == TCA_KIND) {
                    if (strcmp(static_cast<const char *>(RTA_DATA(attribute)), "taprio") == 0) {
                        // Get interface index and use it as a new index to our map.
                        auto interface_index = ((struct tcmsg *) (RTA_DATA(attribute)))->tcm_ifindex;

                        // If there is not a single record of such an interface, create one.
                        if (_time_aware_shaping_rules.count(interface_index) == 0) {
                            _time_aware_shaping_rules[interface_index] = new TAS::TAS();
                        }

                        // Advance one attribute to further analyze.
                        attribute = RTA_NEXT(attribute, attribute_length);

                        // Create a new structure object to gather TAPRIO attribute content.
                        struct rtattr *taprio_attribute[TCA_TAPRIO_ATTR_MAX + 1];
                        parse_rtattr_flags(taprio_attribute, TCA_TAPRIO_ATTR_MAX,
                                           static_cast<struct rtattr *>(RTA_DATA(attribute)),
                                           RTA_PAYLOAD(attribute), NLA_F_NESTED);
                        if (taprio_attribute[TCA_TAPRIO_ATTR_PRIOMAP] != nullptr) {
                            auto *queue_value = (struct tc_mqprio_qopt *) RTA_DATA(
                                    taprio_attribute[TCA_TAPRIO_ATTR_PRIOMAP]);

                            // Collect information on num_tc argument in TAPRIO rule.
                            uint8_t number_of_traffic_classes = queue_value->num_tc;

                            // Collect information on map argument in TAPRIO rule.
                            std::array<uint8_t, TC_PRIO_MAX> priority_to_tc_map{};
                            for (int i = 0; i != TC_PRIO_MAX; ++i) {
                                priority_to_tc_map.at(i) = queue_value->prio_tc_map[i];
                            }

                            // Collect information on queues (count and offsets) argument in TAPRIO rule.
                            auto queues = std::vector<std::pair<uint8_t, uint8_t>>(number_of_traffic_classes,
                                                                                   std::make_pair(0, 0));
                            for (int i = 0; i != queues.size(); ++i) {
                                queues.at(i) = std::make_pair(queue_value->count[i], queue_value->offset[i]);
                            }

                            // Collect information on base-time argument in TAPRIO rule.
                            signed long long int base_time;  // TODO Check this long·ish data type.
                            if (taprio_attribute[TCA_TAPRIO_ATTR_SCHED_BASE_TIME]) {
                                memcpy(&base_time, RTA_DATA(taprio_attribute[TCA_TAPRIO_ATTR_SCHED_BASE_TIME]),
                                       sizeof(base_time));
                            }

                            // Collect information on clockid argument in TAPRIO rule.
                            std::optional<__s32> clock_id;
                            if (taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CLOCKID]) {
                                clock_id = *(__s32 *) RTA_DATA(taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CLOCKID]);
                            }
                            std::string clock_id_string = clock_id.has_value() ? get_clock_name(clock_id.value())
                                                                               : "inexistent";

                            // Collect information on sched-entry arguments in TAPRIO rule.
                            struct rtattr *schedule_pointer = taprio_attribute[TCA_TAPRIO_ATTR_SCHED_ENTRY_LIST];
                            std::vector<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>> schedule;   // Order: index, command, gatemask, interval.
                            struct rtattr *schedule_item;
                            int remainder;
                            if (schedule_pointer != nullptr) {
                                remainder = RTA_PAYLOAD(schedule_pointer);
                                for (schedule_item = (struct rtattr *) RTA_DATA(schedule_pointer); RTA_OK(schedule_item,
                                                                                                          remainder); schedule_item = RTA_NEXT(
                                        schedule_item, remainder)) {
                                    struct rtattr *schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_MAX + 1];
                                    uint32_t index = 0, command = 0, gatemask = 0, interval = 0;
                                    parse_rtattr_flags(schedule_attribute, TCA_TAPRIO_SCHED_ENTRY_MAX,
                                                       (struct rtattr *) (RTA_DATA(schedule_item)),
                                                       RTA_PAYLOAD(schedule_item), NLA_F_NESTED);
                                    if (schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_INDEX]) {
                                        index = *(uint32_t *) RTA_DATA(
                                                schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_INDEX]);
                                    }
                                    if (schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_CMD]) {
                                        command = *(uint32_t *) RTA_DATA(
                                                schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_CMD]);
                                    }
                                    if (schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_GATE_MASK]) {
                                        gatemask = *(uint32_t *) RTA_DATA(
                                                schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_GATE_MASK]);
                                    }
                                    if (schedule_attribute[TCA_TAPRIO_SCHED_ENTRY_INTERVAL]) {
                                        interval = *(uint32_t *) RTA_DATA(
                                                taprio_attribute[TCA_TAPRIO_SCHED_ENTRY_INTERVAL]);
                                    }
                                    schedule.emplace_back(index, command, gatemask, interval);
                                }
                            }

                            // Collect information on flags argument in TAPRIO rule.
                            std::optional<uint32_t> flags;
                            if (taprio_attribute[TCA_TAPRIO_ATTR_FLAGS]) {
                                flags = *(uint32_t *) RTA_DATA(taprio_attribute[TCA_TAPRIO_ATTR_FLAGS]);
                            }

                            // Collect information on txtime-delay argument in TAPRIO rule.
                            std::optional<uint32_t> txtime_delay;
                            if (taprio_attribute[TCA_TAPRIO_ATTR_TXTIME_DELAY]) {
                                txtime_delay = *(uint32_t *) RTA_DATA(taprio_attribute[TCA_TAPRIO_ATTR_TXTIME_DELAY]);
                            }

                            // Collect extra information on cycle time.
                            signed long long int cycle_time;
                            if (taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CYCLE_TIME]) {
                                memcpy(&cycle_time, RTA_DATA(taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CYCLE_TIME]),
                                       sizeof(cycle_time));
                            }

                            // Collect extra information on cycle time extension.
                            signed long long int cycle_time_extension;
                            if (taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CYCLE_TIME_EXTENSION]) {
                                mempcpy(&cycle_time_extension,
                                        RTA_DATA(taprio_attribute[TCA_TAPRIO_ATTR_SCHED_CYCLE_TIME_EXTENSION]),
                                        sizeof(cycle_time_extension));
                            }

                            // Fill TAS::TAS structure with retrieved data.
                            TAS::GateParameterTable gate_parameter_table; // TODO How to fill this?
                            bool gate_enabled = true;
                            uint8_t oper_gate_states =
                                    (uint8_t) (~0) >> (TC_PRIO_MAX - (TC_PRIO_MAX - number_of_traffic_classes));
                            TAS::OperControlList oper_control_list;
                            for (const auto &[index, command, gatemask, interval]: schedule) {
                                oper_control_list.emplace_back(index, static_cast<TAS::TypeOfOperation>(command),
                                                               interval, gatemask);
                            }
                            TAS::OperCycleTime oper_cycle_time = ieee802::RationalGrouping(cycle_time, 1);
                            uint32_t oper_cycle_time_extension = cycle_time_extension;
                            std::chrono::nanoseconds base_time_ns(base_time);
                            auto base_time_s = std::chrono::duration_cast<std::chrono::seconds>(base_time_ns);
                            TAS::OperBaseTime oper_base_time = ieee802::PTPTimeGrouping(base_time_s.count(),
                                                                                        base_time_ns.count());
                            bool config_change = false;
                            timespec current_time_specification;
                            clock_gettime(CLOCK_REALTIME, &current_time_specification);
                            TAS::CurrentTime current_time = ieee802::PTPTimeGrouping(current_time_specification.tv_sec,
                                                                                     current_time_specification.tv_nsec);
                            bool config_pending = false;

                            _time_aware_shaping_rules.at(interface_index) = new TAS::SchedParameters(
                                    std::nullopt,                   // gate_parameter_table
                                    gate_enabled,                   // gate_enabled
                                    oper_gate_states,               // admin_gate_states
                                    oper_gate_states,               // oper_gate_states
                                    oper_control_list,              // admin_control_list
                                    oper_control_list,              // oper_control_list
                                    oper_cycle_time,                // admin_cycle_time
                                    oper_cycle_time,                // oper_cycle_time
                                    oper_cycle_time_extension,      // admin_cycle_time_extension
                                    oper_cycle_time_extension,      // oper_cycle_time_extension
                                    oper_base_time,                 // admin_base_time
                                    oper_base_time,                 // oper_base_time
                                    config_change,                  // config_change
                                    std::nullopt,                   // config_change_time
                                    std::nullopt,                   // tick_granularity
                                    current_time,                   // currentTime
                                    config_pending,                 // config_pending
                                    std::nullopt,                   // config_change_error
                                    std::nullopt,                   // supported_list_max
                                    std::nullopt,                   // supported_cycle_max
                                    std::nullopt);                  // supported_interval_max
                        }
                    }
                }
                attribute = RTA_NEXT(attribute, attribute_length);
            }
        }
        response_header = NLMSG_NEXT(response_header, response_length);
    }
}

void Latency::snap_cbs_via_linux_tc() {
    _credit_based_shaping_rules.clear();

    // Declare structure for kernel request on TC.
    struct {
        struct nlmsghdr header;
        struct tcmsg message;
    } request{};

    // Fill up fields for request.
    request.header.nlmsg_len = sizeof(request);
    request.header.nlmsg_type = RTM_GETQDISC;
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    request.header.nlmsg_seq = time(nullptr);
    request.header.nlmsg_pid = 0;
    request.message.tcm_family = AF_UNSPEC;

    // With the request done, we send it.
    if (send(_socket_descriptor, &request, sizeof(request), 0) < 0) {
        close(_socket_descriptor);
        throw std::ios_base::failure("Netlink socket could not be used to send data in Latency-CBS.");
    }

    // Receive the response and handle it accordingly.
    char response[8192];
    ssize_t response_length = recv(_socket_descriptor, response, sizeof(response), 0);
    if (response_length < 0) {
        close(_socket_descriptor);
        throw std::ios_base::failure("Netlink socket could not be used to receive data in Latency-CBS.");
    }

    // Clear all the rules from the main object map.
    _credit_based_shaping_rules.clear();

    // Parse the response fields.
    auto *response_header = (struct nlmsghdr *) response;
    while (NLMSG_OK(response_header, response_length)) {
        if (response_header->nlmsg_type == RTM_NEWQDISC) {
            auto *content = (struct tcmsg *) NLMSG_DATA(response_header);
            auto *attribute = (struct rtattr *) (content + 1);
            int attribute_length = response_header->nlmsg_len - NLMSG_LENGTH(sizeof(*content));
            while (RTA_OK(attribute, attribute_length)) {
                if (attribute->rta_type == TCA_KIND) {
                    if (strcmp(static_cast<const char *>(RTA_DATA(attribute)), "cbs") == 0) {
                        // Get interface index and use it as a new index to our map.
                        auto interface_index = ((struct tcmsg *) (RTA_DATA(attribute)))->tcm_ifindex;

                        // If there is not a single record of such an interface, create one.
                        if (_credit_based_shaping_rules.count(interface_index) == 0) {
                            _credit_based_shaping_rules[interface_index] = new CBS::CBS();
                        }

                        auto traffic_class = ((struct tcmsg *) (RTA_DATA(attribute)))->tcm_parent;
                        auto idle_slope = ((struct tc_cbs_qopt *) (RTA_DATA(attribute)))->idleslope;
                        _credit_based_shaping_rules[interface_index]->cbsa_parameters->push_back(
                                CBS::CBSAParametersTableEntry({static_cast<uint8_t>(traffic_class), 0, idle_slope}));
                    }
                }
                attribute = RTA_NEXT(attribute, attribute_length);
            }
        }
        response_header = NLMSG_NEXT(response_header, response_length);
    }
}

#endif

Latency::~Latency() {
#ifndef LINUX_TC
    close(_socket_descriptor);
#endif
}