#include "synchronization/ptp/ptp.h"
#include "excelfore-gptp/gptpmasterclock.h"

[[maybe_unused]] static std::vector<std::string> timestamping_labels = {
        "hardware-transmit",      // 0 SOF_TIMESTAMPING_TX_HARDWARE
        "software-transmit",      // 1 SOF_TIMESTAMPING_TX_SOFTWARE
        "hardware-receive",       // 2 SOF_TIMESTAMPING_RX_HARDWARE
        "software-receive",       // 3 SOF_TIMESTAMPING_RX_SOFTWARE
        "software-system-clock",  // 4 SOF_TIMESTAMPING_SOFTWARE
        "hardware-legacy-clock",  // 5 SOF_TIMESTAMPING_SYS_HARDWARE
        "hardware-raw-clock",     // 6 SOF_TIMESTAMPING_RAW_HARDWARE
};

[[maybe_unused]] static std::vector<std::string> transmission_types = {
        "off",            // HWTSTAMP_TX_OFF
        "on",             // HWTSTAMP_TX_ON
        "one-step-sync",  // HWTSTAMP_TX_ONESTEP_SYNC
};

const std::vector<std::string> reception_filters = {
        "none",                // HWTSTAMP_FILTER_NONE
        "all",                 // HWTSTAMP_FILTER_ALL
        "some",                // HWTSTAMP_FILTER_SOME
        "ptpv1-l4-event",      // HWTSTAMP_FILTER_PTP_V1_L4_EVENT
        "ptpv1-l4-sync",       // HWTSTAMP_FILTER_PTP_V1_L4_SYNC
        "ptpv1-l4-delay-req",  // HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ
        "ptpv2-l4-event",      // HWTSTAMP_FILTER_PTP_V2_L4_EVENT
        "ptpv2-l4-sync",       // HWTSTAMP_FILTER_PTP_V2_L4_SYNC
        "ptpv2-l4-delay-req",  // HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ
        "ptpv2-l2-event",      // HWTSTAMP_FILTER_PTP_V2_L2_EVENT
        "ptpv2-l2-sync",       // HWTSTAMP_FILTER_PTP_V2_L2_SYNC
        "ptpv2-l2-delay-req",  // HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ
        "ptpv2-event",         // HWTSTAMP_FILTER_PTP_V2_EVENT
        "ptpv2-sync",          // HWTSTAMP_FILTER_PTP_V2_SYNC
        "ptpv2-delay-req",     // HWTSTAMP_FILTER_PTP_V2_DELAY_REQ
        "ntp-all",             // HWTSTAMP_FILTER_NTP_ALL
};

std::map<std::string, std::vector<std::string>> get_ptp_capabilities(const std::string& interface) {
    struct ethtool_command_context command_context {};

    // Check if interface has a valid length name.
    if (interface.length() >= IFNAMSIZ) {
        // TODO Handle this error with an exception throw.
    }
    command_context.devname = interface.c_str();
    memset(&command_context.ifr, 0, sizeof(command_context.ifr));
    strncpy(command_context.ifr.ifr_name, command_context.devname, interface.size());

    // Create a socket and retrieve its file descriptor.
    command_context.fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (command_context.fd < 0) {
        // TODO Handle this error with an exception throw.
    }

    // Create the command timestamp info request to kernel.
    struct ethtool_ts_info information_request {};
    information_request.cmd = ETHTOOL_GET_TS_INFO;

    // Send IOCTL request with the gathered information.
    command_context.ifr.ifr_data = &information_request;
    int error = ioctl(command_context.fd, IOCTL_ETHTOOL_REQ, &command_context.ifr);
    if (error < 0) {
        // TODO Handle this error with an exception throw.
    }

    // Parse the information left in the information request.
    std::map<std::string, std::vector<std::string>> information;

    std::vector<std::string> capabilities;
    for (int i = 0; i < timestamping_labels.size(); ++i) {
        if (information_request.so_timestamping & (1 << i)) {
            capabilities.push_back(timestamping_labels.at(i));
        }
    }
    information["capabilities"] = capabilities;

    std::vector<std::string> ptp_hardware_clock;
    if (information_request.phc_index > 0) {
        ptp_hardware_clock.push_back(std::to_string(information_request.phc_index));
    }
    information["ptp_hardware_clock"] = ptp_hardware_clock;

    std::vector<std::string> hardware_transmit_ts_modes;
    for (int i = 0; i < transmission_types.size(); ++i) {
        if (information_request.tx_types & (1 << i)) {
            hardware_transmit_ts_modes.push_back(transmission_types.at(i));
        }
    }
    information["hardware_transmit_ts_mode"] = hardware_transmit_ts_modes;

    std::vector<std::string> hardware_receive_filter_modes;
    for (int i = 0; i < reception_filters.size(); ++i) {
        if (information_request.rx_filters & (1 << i)) {
            hardware_receive_filter_modes.push_back(reception_filters.at(i));
        }
    }
    information["hardware_receive_filter_modes"] = hardware_receive_filter_modes;

    return information;
}

PTPCapability capability_level_in(const std::map<std::string, std::vector<std::string>>& information) {
    // If there is no capability, then it is clearly uncapable.
    if (information.at("capabilities").empty()) {
        return PTPCapability::UNCAPABLE;
    } else {
        PTPCapability last_state = PTPCapability::UNCAPABLE;
        int counter = 0;
        for (const auto& capability : information.at("capabilities")) {
            for (int i = 0; i < timestamping_labels.size(); ++i) {
                if (capability == timestamping_labels.at(i)) {
                    counter |= (1 << i);
                    break;
                }
            }
        }
        if ((counter & HW_CAPABILITY_MASK) == counter) {
            last_state = PTPCapability::HARDWARE;
        } else if ((counter & SW_CAPABILITY_MASK) == counter) {
            last_state = PTPCapability::SOFTWARE_ONLY;
        }

        if (information.at("ptp_hardware_clock").empty()) {
            return PTPCapability::SOFTWARE_ONLY;
        }

        if (last_state == PTPCapability::HARDWARE) {
            int modes = 0;
            for (const auto& mode : information.at("hardware_transmit_ts_mode")) {
                for (int i = 0; i < transmission_types.size(); ++i) {
                    if (mode == transmission_types.at(i)) {
                        modes |= (1 << i);
                        break;
                    }
                }
            }
            if ((modes & HW_TX_TS_MODE_MASK) != modes) {
                last_state = PTPCapability::SOFTWARE_ONLY;
            }
        }

        if (last_state == PTPCapability::HARDWARE) {
            int filters = 0;
            for (const auto& filter : information.at("hardware_receive_filter_modes")) {
                for (int i = 0; i < reception_filters.size(); ++i) {
                    if (filter == reception_filters.at(i)) {
                        filters |= (1 << i);
                        break;
                    }
                }
            }

            if ((filters & HW_RX_FILTER_MASK) != filters) {
                last_state = PTPCapability::SOFTWARE_ONLY;
            }
        }

        return last_state;
    }
}

PTPCapability ptp_capability_type(const std::string& interface_name) {
    return capability_level_in(get_ptp_capabilities(interface_name));
}

void PTP::snap() {
    // Verify if ptp4l is running. If not, quit.
    if (stat("/var/run/ptp4l", &buffer) == 0) {
        // Get configuration from linuxptp.
        struct config* pmc_configuration = config_create();
        if (!pmc_configuration) {
            // TODO Handle this error with an exception throw.
        }

        // Set flags to allow polling ptp4l.
        if (config_set_int(pmc_configuration, "network_transport", TRANS_UDS)) {
            // TODO Handle this error with an exception throw.
        }

        // Specify initialized variables.
        uint8_t boundary_hops = 0;
        auto transport_type = TRANS_UDS;
        auto transport_specific = config_get_int(pmc_configuration, nullptr, "transportSpecific") << 4;
        auto domain_number = config_get_int(pmc_configuration, nullptr, "domainNumber");
        auto interface_name = "/var/run/pmc." + std::to_string(getpid());

        // Create the pmc-relatable representation.
        auto pmc_object = pmc_create(pmc_configuration, transport_type, interface_name.c_str(), boundary_hops,
                                     domain_number, transport_specific, 0);
        if (!pmc_object) {
            // TODO Handle this error with an exception throw.
        }

        // Prepare polling.
        struct pollfd pollfd {};

        // Get TIME_STATUS_NP data.
        pollfd.fd = pmc_get_transport_fd(pmc_object);
        pollfd.events = POLLIN | POLLPRI | POLLOUT;
        int count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        // Parse and perform polling events.
        if (pollfd.revents & POLLOUT) {
            if (pmc_do_command(pmc_object, std::string("GET TIME_STATUS_NP").data())) {
                // TODO Handle this error with an exception throw.
            }
        }
        pollfd.events = POLLIN | POLLPRI;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        if (pollfd.revents & (POLLIN)) {
            struct ptp_message* message = pmc_recv(pmc_object);
            if (message) {
                auto* time_status = (struct time_status_np*) ((struct management_tlv*) message->management.suffix)->data;
                _master_offset = time_status->master_offset;
                _ingress_time = time_status->ingress_time;
                _cumulative_scaled_rate_offset = (time_status->cumulativeScaledRateOffset + 0.0) / ((double) (1ULL << 41));
                _scaled_last_grand_master_phase_change = time_status->scaledLastGmPhaseChange;
                _grand_master_time_base_indicator = time_status->gmTimeBaseIndicator;
                _last_grand_master_phase_change_ns_msb = time_status->lastGmPhaseChange.nanoseconds_msb;
                _last_grand_master_phase_change_ns_lsb = time_status->lastGmPhaseChange.nanoseconds_lsb;
                _last_grand_master_phase_change_fractional_ns = time_status->lastGmPhaseChange.fractional_nanoseconds;
                _grand_master_present = time_status->gmPresent != 0;
                _grand_master_identity = cid2str(&time_status->gmIdentity);
            }
        }

        // Get GRANDMASTER_SETTINGS_NP data.
        pollfd.fd = pmc_get_transport_fd(pmc_object);
        pollfd.events = POLLIN | POLLPRI | POLLOUT;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        // Parse and perform polling events.
        if (pollfd.revents & POLLOUT) {
            if (pmc_do_command(pmc_object, std::string("GET GRANDMASTER_SETTINGS_NP").data())) {
                // TODO Handle this error with an exception throw.
            }
        }
        pollfd.events = POLLIN | POLLPRI;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        if (pollfd.revents & POLLIN) {
            struct ptp_message* message = pmc_recv(pmc_object);
            if (message) {
                auto gm_settings =
                        (struct grandmaster_settings_np*) ((struct management_tlv*) message->management.suffix)->data;
                _grand_master_clock_class = gm_settings->clockQuality.clockClass;
                _grand_master_clock_accuracy = gm_settings->clockQuality.clockAccuracy;
                _grand_master_offset_scaled_log_variance = gm_settings->clockQuality.offsetScaledLogVariance;
                _grand_master_current_utc_offset = gm_settings->utc_offset;
                _grand_master_leap_61 = gm_settings->time_flags & LEAP_61;
                _grand_master_leap_59 = gm_settings->time_flags & LEAP_59;
                _grand_master_current_utc_offset_valid = gm_settings->time_flags & UTC_OFF_VALID;
                _grand_master_ptp_timescale = gm_settings->time_flags & PTP_TIMESCALE;
                _grand_master_time_traceable = gm_settings->time_flags & TIME_TRACEABLE;
                _grand_master_frequency_traceable = gm_settings->time_flags & FREQ_TRACEABLE;
                _grand_master_time_source = gm_settings->time_source;
            }
        }

        // Get PARENT_DATA_SET data.
        pollfd.fd = pmc_get_transport_fd(pmc_object);
        pollfd.events = POLLIN | POLLPRI | POLLOUT;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        // Parse and perform polling events.
        if (pollfd.revents & POLLOUT) {
            if (pmc_do_command(pmc_object, std::string("GET PARENT_DATA_SET").data())) {
                // TODO Handle this error with an exception throw.
            }
        }
        pollfd.events = POLLIN | POLLPRI;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        if (pollfd.revents & POLLIN) {
            struct ptp_message* message = pmc_recv(pmc_object);
            if (message) {
                auto parent_ds = (struct parentDS*) ((struct management_tlv*) message->management.suffix)->data;
                _parent_port_identity = pid2str(&parent_ds->parentPortIdentity);
                _parent_stats = parent_ds->parentStats;
                _observed_parent_offset_scaled_log_variance = parent_ds->observedParentOffsetScaledLogVariance;
                _observed_parent_clock_phase_change_rate = parent_ds->observedParentClockPhaseChangeRate;
                _grandmaster_priority_1 = parent_ds->grandmasterPriority1;
                _grandmaster_priority_2 = parent_ds->grandmasterPriority2;
            }
        }

        // Get CLOCK_DESCRIPTION data.
        pollfd.fd = pmc_get_transport_fd(pmc_object);
        pollfd.events = POLLIN | POLLPRI | POLLOUT;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        // Parse and perform polling events.
        if (pollfd.revents & POLLOUT) {
            if (pmc_do_command(pmc_object, std::string("GET CLOCK_DESCRIPTION").data())) {
                // TODO Handle this error with an exception throw.
            }
        }
        pollfd.events = POLLIN | POLLPRI;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        if (pollfd.revents & POLLIN) {
            struct ptp_message* message = pmc_recv(pmc_object);
            if (message) {
                auto details = TAILQ_FIRST(&message->tlv_list);
                auto clock_description = &details->cd;
                _clock_type = align16(clock_description->clockType);
                _physical_layer_protocol = text2str(clock_description->physicalLayerProtocol);
                _physical_address = bin2str(clock_description->physicalAddress->address,
                                            align16(&clock_description->physicalAddress->length));
                _protocol_address = align16(&clock_description->protocolAddress->networkProtocol);
                _manufacturer_id = bin2str(clock_description->manufacturerIdentity, OUI_LEN);
                _product_description = text2str(clock_description->productDescription);
                _revision_data = text2str(clock_description->revisionData);
                _user_description = text2str(clock_description->userDescription);
                _profile_id = bin2str(clock_description->profileIdentity, PROFILE_ID_LEN);
            }
        }

        // Get CURRENT_DATA_SET data.
        pollfd.fd = pmc_get_transport_fd(pmc_object);
        pollfd.events = POLLIN | POLLPRI | POLLOUT;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        // Parse and perform polling events.
        if (pollfd.revents & POLLOUT) {
            if (pmc_do_command(pmc_object, std::string("GET CURRENT_DATA_SET").data())) {
                // TODO Handle this error with an exception throw.
            }
        }
        pollfd.events = POLLIN | POLLPRI;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        if (pollfd.revents & POLLIN) {
            struct ptp_message* message = pmc_recv(pmc_object);
            if (message) {
                auto current_ds = (struct currentDS*) ((struct management_tlv*) message->management.suffix)->data;
                _steps_removed = current_ds->stepsRemoved;
                _offset_from_master = current_ds->offsetFromMaster / 65536.0;
                _mean_path_delay = current_ds->meanPathDelay / 65536.0;
            }
        }

        // Get PORT_DATA_SET data.
        pollfd.fd = pmc_get_transport_fd(pmc_object);
        pollfd.events = POLLIN | POLLPRI | POLLOUT;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        // Parse and perform polling events.
        if (pollfd.revents & POLLOUT) {
            if (pmc_do_command(pmc_object, std::string("GET PORT_DATA_SET").data())) {
                // TODO Handle this error with an exception throw.
            }
        }
        pollfd.events = POLLIN | POLLPRI;
        count = poll(&pollfd, 1, 100);
        if (count < 0) {
            // TODO Handle this error with an exception throw.
        }

        if (pollfd.revents & POLLIN) {
            struct ptp_message* message = pmc_recv(pmc_object);
            if (message) {
                auto port_ds = (struct portDS*) ((struct management_tlv*) message->management.suffix)->data;
                _port_identity = pid2str(&port_ds->portIdentity);
                _port_state = ps_str[port_ds->portState];
                _log_min_delay_request_interval = port_ds->logMinDelayReqInterval;
                _log_min_peer_delay_request_interval = port_ds->peerMeanPathDelay >> 16;
                _log_announce_interval = port_ds->logAnnounceInterval;
                _announce_receipt_timeout = port_ds->announceReceiptTimeout;
                _log_sync_interval = port_ds->logSyncInterval;
                _delay_mechanism = port_ds->delayMechanism;
                _log_min_peer_delay_request_interval = port_ds->logMinPdelayReqInterval;
                _version_number = port_ds->versionNumber & MAJOR_VERSION_MASK;
            }
        }

        // Clean used structures.
        pmc_destroy(pmc_object);
        msg_cleanup();
        config_destroy(pmc_configuration);
        return;
    }
#ifdef _EXCELFORE_GPTP
    gptpmasterclock_getts64();
#endif

}

rapidjson::Document PTP::json() const {
    // Create a JSON document.
    rapidjson::Document json_document;
    json_document.SetObject();
    auto& allocator = json_document.GetAllocator();

    // Create a temporary value.
    rapidjson::Value value;

    value.SetBool(_running);
    json_document.AddMember("running", value, allocator);
    value.SetString(rapidjson::GenericStringRef(mechanism_to_string(_mechanism).c_str()));
    json_document.AddMember("running-on", value, allocator);
    if (_running) {
        value.SetBool(_applying_on_system);
        json_document.AddMember("applying-on-system", value, allocator);
        value.SetInt64(_master_offset);
        json_document.AddMember("master-offset", value, allocator);
        value.SetInt64(_ingress_time);
        json_document.AddMember("ingress-time", value, allocator);
        value.SetDouble(_cumulative_scaled_rate_offset);
        json_document.AddMember("cumulative-scaled-rate-offset", value, allocator);
        value.SetInt(_scaled_last_grand_master_phase_change);
        json_document.AddMember("scaled-last-grand-master-phase-change", value, allocator);
        value.SetUint(_grand_master_time_base_indicator);
        json_document.AddMember("grand-master-time-base-indicator", value, allocator);
        value.SetUint(_last_grand_master_phase_change_ns_msb);
        json_document.AddMember("last-grand-master-phase-change-ns-msb", value, allocator);
        value.SetUint(_last_grand_master_phase_change_ns_lsb);
        json_document.AddMember("last-grand-master-phase-change-ns-lsb", value, allocator);
        value.SetUint(_last_grand_master_phase_change_fractional_ns);
        json_document.AddMember("last-grand-master-phase-change-fractional-ns", value, allocator);
        value.SetBool(_grand_master_present);
        json_document.AddMember("grand-master-present", value, allocator);
        value.SetUint(_grand_master_clock_class);
        json_document.AddMember("grand-master-clock-class", value, allocator);
        value.SetUint(_grand_master_clock_accuracy);
        json_document.AddMember("grand-master-clock-accuracy", value, allocator);
        value.SetUint(_grand_master_offset_scaled_log_variance);
        json_document.AddMember("grand-master-offset-scaled-log-variance", value, allocator);
        value.SetUint(_grand_master_current_utc_offset);
        json_document.AddMember("grand-master-current-utc-offset", value, allocator);
        value.SetBool(_grand_master_leap_61);
        json_document.AddMember("grand-master-leap-61", value, allocator);
        value.SetBool(_grand_master_leap_59);
        json_document.AddMember("grand-master-leap-59", value, allocator);
        value.SetBool(_grand_master_current_utc_offset_valid);
        json_document.AddMember("grand-master-current-utc-offset-valid", value, allocator);
        value.SetBool(_grand_master_ptp_timescale);
        json_document.AddMember("grand-master-ptp-timescale", value, allocator);
        value.SetBool(_grand_master_time_traceable);
        json_document.AddMember("grand-master-time-traceable", value, allocator);
        value.SetBool(_grand_master_frequency_traceable);
        json_document.AddMember("grand-master-frequency-traceable", value, allocator);
        value.SetUint(_grand_master_time_source);
        json_document.AddMember("grand-master-time-source", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_grand_master_identity.c_str()));
        json_document.AddMember("grand-master-identity", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_parent_port_identity.c_str()));
        json_document.AddMember("parent-port-identity", value, allocator);
        value.SetUint(_parent_stats);
        json_document.AddMember("parent-stats", value, allocator);
        value.SetUint(_observed_parent_offset_scaled_log_variance);
        json_document.AddMember("observed-parent-offset-scaled-log-variance", value, allocator);
        value.SetInt(_observed_parent_clock_phase_change_rate);
        json_document.AddMember("observed-parent-clock-phase-change-rate", value, allocator);
        value.SetUint(_grandmaster_priority_1);
        json_document.AddMember("grand-master-priority-1", value, allocator);
        value.SetUint(_grandmaster_priority_2);
        json_document.AddMember("grand-master-priority-2", value, allocator);
        value.SetUint(_steps_removed);
        json_document.AddMember("steps-removed", value, allocator);
        value.SetDouble(_offset_from_master);
        json_document.AddMember("offset-from-master", value, allocator);
        value.SetDouble(_mean_path_delay);
        json_document.AddMember("mean-path-delay", value, allocator);
        value.SetUint(_clock_type);
        json_document.AddMember("clock-type", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_physical_layer_protocol.c_str()));
        json_document.AddMember("physical-layer-protocol", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_physical_address.c_str()));
        json_document.AddMember("physical-address", value, allocator);
        value.SetUint(_protocol_address);
        json_document.AddMember("protocol-address", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_manufacturer_id.c_str()));
        json_document.AddMember("manufacturer-id", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_product_description.c_str()));
        json_document.AddMember("product-description", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_revision_data.c_str()));
        json_document.AddMember("revision-data", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_user_description.c_str()));
        json_document.AddMember("user-description", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_profile_id.c_str()));
        json_document.AddMember("profile-id", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_port_identity.c_str()));
        json_document.AddMember("port-identity", value, allocator);
        value.SetString(rapidjson::GenericStringRef(_port_state.c_str()));
        json_document.AddMember("port-state", value, allocator);
        value.SetInt(_log_min_delay_request_interval);
        json_document.AddMember("log-min-delay-request-interval", value, allocator);
        value.SetInt64(_peer_mean_path_delay);
        json_document.AddMember("peer-mean-path-delay", value, allocator);
        value.SetInt(_log_announce_interval);
        json_document.AddMember("log-announce-interval", value, allocator);
        value.SetUint(_announce_receipt_timeout);
        json_document.AddMember("announce-receipt-timeout", value, allocator);
        value.SetInt(_log_sync_interval);
        json_document.AddMember("log-sync-interval", value, allocator);
        value.SetUint(_delay_mechanism);
        json_document.AddMember("delay-mechanism", value, allocator);
        value.SetInt(_log_min_peer_delay_request_interval);
        json_document.AddMember("log-min-peer-delay-request-interval", value, allocator);
        value.SetUint(_version_number);
        json_document.AddMember("version-number", value, allocator);
    }

    return json_document;
}

#ifndef RELEASE_TARGET
void debug_all_interfaces_ptp_capabilities() {
    struct ifaddrs* address;
    struct ifaddrs* temporary_address;

    getifaddrs(&address);
    temporary_address = address;

    while (temporary_address) {
        if (temporary_address->ifa_addr && temporary_address->ifa_addr->sa_family == AF_PACKET) {
            auto type = ptp_capability_type(temporary_address->ifa_name);
            switch (type) {
                case PTPCapability::HARDWARE:
                    std::cout << temporary_address->ifa_name << " capability is full-hardware." << std::endl;
                    break;
                case PTPCapability::SOFTWARE_ONLY:
                    std::cout << temporary_address->ifa_name << " capability is only by software." << std::endl;
                    break;
                default:
                    std::cout << temporary_address->ifa_data << " is uncapable of handling with PTP." << std::endl;
            }
        }

        temporary_address = temporary_address->ifa_next;
    }

    free(address);
}
#endif