#include "structs/resources.h"

#ifdef FROM_SIMUZILLA
Resources* Resources::get_instance(const std::function<const std::vector<int>*()>& topology, unsigned int own_address, const std::string& logger_name) {
    return new Resources(topology, own_address, logger_name);
}
#else
Resources* Resources::get_instance(const std::string& logger_name) {
    static Resources* instance = new Resources(logger_name);
    return instance;
}
#endif

float Resources::estimate_bid(const RequestingCapabilities& capabilities) const {
    // FIXME Talvez cortar este método?

    return -1;
}

double Resources::estimate_bid(const Reservation& reservation) {
    // Phase 1: Bare-metal criterium for reservation.
    double node_resources = bare_metal_resource_assessment(reservation.requirements());

    // Phase 2: Current capability for performance of requirement list.
    double current_resources = current_resource_assessment(reservation.requirements());

    // Phase 3: Fairness.
    double fairness = reservation.priority() / RANK_MAX_PRIORITY;

    // Phase 4: Proximity criterium.
    double proximity = proximity_assessment(reservation.listener(), reservation.listener_length());

    // Phase 5: Historical Performance criterium.
    double hysteresis = hysteresis_assessment(reservation);

    return node_resources * (proximity + hysteresis) / 2 * current_resources * (fairness / RANK_MAX_PRIORITY);
}

double Resources::bare_metal_resource_assessment(const RequestingCapabilities &requirements) const {
    // For each required item, check its bare-metal resource.
    for (const auto& [item_order, item]: requirements.ordered_items()) {
        const CapabilityItemType& item_key = item.first;
        const std::any& item_value = item.second;

        if (not item_value.has_value()) {
            return 0.0;
        }

        switch (item_key) {
            case CapabilityItemType::TIME:
                break;
            case CapabilityItemType::TIME_TAS:
                break;
            case CapabilityItemType::TIME_CBS:
                break;
            case CapabilityItemType::NET:
                break;
            case CapabilityItemType::NET_BANDWIDTH:
                break;
            case CapabilityItemType::NET_DDS:
                break;
            case CapabilityItemType::COMP:
                break;
            case CapabilityItemType::COMP_CPU:
                if (std::any_cast<uint>(item_value) >= _current_capabilities->bare_metal_cpu_cores()) {
                    return 0.0;
                }
                break;
            case CapabilityItemType::COMP_MEMORY:
                if (std::any_cast<uint>(item_value) >= _current_capabilities->bare_metal_memory()) {
                    return 0.0;
                }
                break;
            case CapabilityItemType::UNSPECIFIED:
                break;
        }
    }

    return 1.0;
}

double Resources::current_resource_assessment(const RequestingCapabilities &requirements) const {
    // For each requirement, individually-assess it and sum it at the end, by means of a function.
    std::vector<double> individual_assessments{};

    for (const auto& [item_order, item]: requirements.ordered_items()) {
        const CapabilityItemType& item_key = item.first;
        const std::any& item_value = item.second;

        if (not item_value.has_value()) {
            return 0.0;
        }

        switch (item_key) {
            case CapabilityItemType::TIME:
                break;
            case CapabilityItemType::TIME_TAS:
                break;
            case CapabilityItemType::TIME_CBS:
                break;
            case CapabilityItemType::NET:
                break;
            case CapabilityItemType::NET_BANDWIDTH:
                break;
            case CapabilityItemType::NET_DDS:
                break;
            case CapabilityItemType::COMP:
                break;
            case CapabilityItemType::COMP_CPU: {
                const auto& value = _current_capabilities->current_cpu_cores();
                if (value == 0) {
                    return 0.0;
                }
                individual_assessments.push_back(value);
            }
                break;
            case CapabilityItemType::COMP_MEMORY: {
                const auto& value = _current_capabilities->current_memory();
                if (value == 0) {
                    return 0.0;
                }
                individual_assessments.push_back(value);
            }
                break;
            case CapabilityItemType::UNSPECIFIED:
                break;
        }
    }

    std::function<double(const std::vector<double>&)> alpha;
    alpha = [&](const std::vector<double>& requirements) -> double {
        if (requirements.size() == 1) {
            return requirements.front();
        }
        std::vector<double> requirements_subvector;
        std::copy(requirements.begin()+1, requirements.end(), std::back_inserter(requirements_subvector));
        return RANK_CURRENT_RESOURCES_EVAL_THRESHOLD*requirements.front() + (1-RANK_CURRENT_RESOURCES_EVAL_THRESHOLD)*alpha(requirements_subvector);
    };

    return alpha(individual_assessments);
}

double Resources::proximity_assessment(const std::array<uint8_t, 16> &target, uint8_t target_length) {
    std::vector<uint8_t> cut_target;
    std::copy_n(target.begin(), target_length, std::back_inserter(cut_target));

    double hops = proximity_hops_assessment(cut_target, true);
    double rtt = proximity_rtt_assessment(cut_target, true);
    double pdv = proximity_pdv_assessment(cut_target, true);
    double pl = proximity_pl_assessment(cut_target, true);

    return (hops+rtt+pdv+pl)/4.0;
}

double Resources::hysteresis_assessment(const Reservation &reservation) const {

    return 1.0;
}

double Resources::proximity_hops_assessment(const std::vector<uint8_t> &target, bool update) {
    std::string key = std::string(target.begin(), target.end());

    if (_proximity_metrics.count(key)) {
        if (update) std::get<0>(_proximity_metrics[key]) = std::chrono::system_clock::now().time_since_epoch().count();
        std::get<1>(_proximity_metrics[key]) = number_of_hops_to(target);
    } else {
        auto entry = std::make_tuple<long, double, double, double, double>(std::chrono::system_clock::now().time_since_epoch().count(), number_of_hops_to(target), NAN, NAN, NAN);
        _proximity_metrics.insert({key, entry});
    }

    std::function<double(double)> phi_2;
    phi_2 = [&](double metric) -> double {
        return pow(2, -floor(metric));
    };

    return phi_2(std::get<1>(_proximity_metrics[key]));
}

double Resources::proximity_rtt_assessment(const std::vector<uint8_t> &target, bool update) {
    std::string key = std::string(target.begin(), target.end());

    if (_proximity_metrics.count(key)) {
        if (update) std::get<0>(_proximity_metrics[key]) = std::chrono::system_clock::now().time_since_epoch().count();
        std::get<2>(_proximity_metrics[key]) = round_trip_time_to(target);
    } else {
        auto entry = std::make_tuple<long, double, double, double, double>(std::chrono::system_clock::now().time_since_epoch().count(), NAN, round_trip_time_to(target), NAN, NAN);
        _proximity_metrics.insert({key, entry});
    }

    std::function<double(double)> phi_1;
    phi_1 = [&](double metric) -> double {
        if (metric <= RANK_PROXIMITY_EVAL_RTT_THRESHOLD) {
            return 1.0;
        }
        return pow(2, -((metric - RANK_PROXIMITY_EVAL_RTT_THRESHOLD) / RANK_PROXIMITY_EVAL_RTT_THRESHOLD));
    };

    return phi_1(std::get<2>(_proximity_metrics[key]));
}

double Resources::proximity_pdv_assessment(const std::vector<uint8_t> &target, bool update) {
    std::string key = std::string(target.begin(), target.end());

    if (_proximity_metrics.count(key)) {
        if (update) std::get<0>(_proximity_metrics[key]) = std::chrono::system_clock::now().time_since_epoch().count();
        std::get<3>(_proximity_metrics[key]) = packet_delay_variation_to(target);
    } else {
        auto entry = std::make_tuple<long, double, double, double, double>(std::chrono::system_clock::now().time_since_epoch().count(), NAN, NAN, packet_delay_variation_to(target), NAN);
        _proximity_metrics.insert({key, entry});
    }

    std::function<double(double)> phi_3;
    phi_3 = [&](double metric) -> double {
        if (metric <= RANK_PROXIMITY_EVAL_PDV_THRESHOLD) {
            return 1.0;
        } else if (metric > RANK_PROXIMITY_EVAL_PDV_THRESHOLD+(RANK_PROXIMITY_EVAL_PDV_THRESHOLD*RANK_PROXIMITY_EVAL_PDV_TOLERANCE)) {
            return 0.0;
        } else {
            return -(metric-RANK_PROXIMITY_EVAL_PDV_THRESHOLD)/(RANK_PROXIMITY_EVAL_PDV_THRESHOLD+RANK_PROXIMITY_EVAL_PDV_THRESHOLD*RANK_PROXIMITY_EVAL_PDV_TOLERANCE) + 1.0;
        }
    };

    return phi_3(std::get<3>(_proximity_metrics[key]));
}

double Resources::proximity_pl_assessment(const std::vector<uint8_t> &target, bool update) {
    std::string key = std::string(target.begin(), target.end());

    if (_proximity_metrics.count(key)) {
        if (update) std::get<0>(_proximity_metrics[key]) = std::chrono::system_clock::now().time_since_epoch().count();
        std::get<4>(_proximity_metrics[key]) = packet_loss_to(target);
    } else {
        auto entry = std::make_tuple<long, double, double, double, double>(std::chrono::system_clock::now().time_since_epoch().count(), NAN, NAN, NAN, packet_loss_to(target));
        _proximity_metrics.insert({key, entry});
    }

    std::function<double(double)> phi_4;
    phi_4 = [&](double metric) -> double {
        if (metric > RANK_PROXIMITY_EVAL_PL_THRESHOLD) {
            return 0.0;
        }

#ifdef RANK_PROXIMITY_EVAL_PL_STEEP
        return (0.75/pow(RANK_PROXIMITY_EVAL_PL_THRESHOLD, 2))*pow((metric - RANK_PROXIMITY_EVAL_PL_THRESHOLD), 2) + 0.25;
#elif RANK_PROXIMITY_EVAL_PL_LINEAR
        return -0.75*pow(RANK_PROXIMITY_EVAL_PL_THRESHOLD, -1)*metric + 1;
#elif RANK_PROXIMITY_EVAL_PL_SMOOTH
        return -(0.75/pow(RANK_PROXIMITY_EVAL_PL_THRESHOLD, 2))*pow(metric, 2) + 1;
#endif
    };

    return phi_4(std::get<4>(_proximity_metrics[key]));
}

Reservation* Resources::available_for_performance(const Reservation& statement, uint8_t priority) {
    // Check if there are no resources to perform this statement.
    if (bare_metal_resource_assessment(statement.requirements()) == 0) {
        return nullptr;
    } else {
        // Check if currently there are no resources to perform this statement.
        if (current_resource_assessment(statement.requirements()) == 0) {
            // Otherwise, then check if there is a reservation with lower priority that could be unconsidered.
            auto reservation_pointer = std::min_element(_reservations.begin(), _reservations.end());

            // If the list is empty, then the pointer will reach the end, returning nullptr as the requirements could not be
            // performed.
            if (reservation_pointer == _reservations.end()) {
                return nullptr;
            }

            // If the minimum priority is higher or equal than the requested one, terminate will nullptr.
            if (reservation_pointer->priority() >= priority) {
                return nullptr;
            }

            // As the priority is lower than requested, replenish the found reservation, replenishing it from the network.
            reservation_pointer->replenish();

            // Create a new reservation with the given statement where the minimum reservation was replenished.
            *reservation_pointer = Reservation(statement);

            // Set the reservation to be waiting for the sacrifice.
            reservation_pointer->wait_for_sacrifice();

            // Return the location of such a reservation.
            return &(*reservation_pointer);
        } else {
            return new Reservation(statement);
        }
    }
}

Resources* Resources::replenish_reservation(Reservation* reservation) {
    // Find reservation in reservations and if no such reservation was found, then throw exception.
    auto found_reservation = std::find(_reservations.begin(), _reservations.end(), *reservation);
    if (found_reservation == _reservations.end()) {
        throw std::exception();  // TODO
    }

    // Pass reservation state to replenish.
    reservation->replenish();

    // Remove reservation in the set of reservations.
    _reservations.erase(found_reservation);

    return this;
}

Resources* Resources::mark_reservation(Reservation* reservation) {
    // Find reservation in reservations and if no such reservation was found, then throw exception.
    auto found_reservation = std::find(_reservations.begin(), _reservations.end(), *reservation);
    if (found_reservation == _reservations.end()) {
        throw std::exception();  // TODO
    }

    // Remove reservation in the set of reservations.
    _reservations.erase(found_reservation);

    // Mark reservation as reserved.
    reservation->mark_reserved();

    // Add the modified reservation to the set of reservations.
    _reservations.push_back(*reservation);
    _reservations.sort();

    return this;
}

Resources* Resources::mark_pre_reservation(Reservation* reservation) {
    // Find reservation in reservations and if no such reservation was found, then throw exception.
    auto found_reservation = std::find(_reservations.begin(), _reservations.end(), *reservation);
    if (found_reservation == _reservations.end()) {
        reservation->pre_reserve();
        _reservations.push_back(*reservation);
        _reservations.sort();
        return this;
    }

    // Remove reservation in the set of reservations.
    _reservations.erase(found_reservation);

    // Mark reservation as pre-reserved.
    reservation->mark_pre_reserved();

    // Add the modified reservation to the set of reservations.
    _reservations.push_back(*reservation);
    _reservations.sort();

    return this;
}

std::list<Reservation> Resources::reservations() const {
    return _reservations;
}

size_t Resources::reservations_size() const {
    return _reservations.size();
}

Resources* Resources::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

Resources* Resources::stop() {
    if (!_running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool Resources::is_running() const {
    return _running;
}

void Resources::operator()() {
    while (_running) {
        // Update information on current capabilities.
        _current_capabilities->update();
        _logger->debug("[Resources] {}", _current_capabilities->display());

        // Update known instances for proximity.
        std::vector<std::string> keys_to_remove;
        for (auto& [target, metrics]: _proximity_metrics) {
            long& timestamp = std::get<0>(metrics);
            double& hops = std::get<1>(metrics);
            double& rtt = std::get<2>(metrics);
            double& pdv = std::get<3>(metrics);
            double& pl = std::get<4>(metrics);

            auto now = std::chrono::system_clock::now().time_since_epoch().count();
            if (now - timestamp >= RANK_DURATION_OLD) {
                keys_to_remove.push_back(target);
                _logger->debug("[Resources] Removed target {} in proximity assessment due to entry not being updated in more than {} seconds.", target, RANK_DURATION_OLD);
            } else {
                auto vector_target = std::vector<uint8_t>(target.begin(), target.end());
                hops = proximity_hops_assessment(vector_target);
                rtt = proximity_hops_assessment(vector_target);
                pdv = proximity_hops_assessment(vector_target);
                pl = proximity_hops_assessment(vector_target);

                _logger->debug("[Resources] Renew proximity assessment for {}: {} hops, RTT of {} ms, PDV of {} ms, and {}% of PL.", target, hops, rtt, pdv, pl);
            }
        }

        // Remove old targets.
        for (const auto& target: keys_to_remove) {
            _proximity_metrics.erase(target);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }
}

Resources::~Resources() {
}

#ifdef FROM_SIMUZILLA
Resources::Resources(const std::function<const std::vector<int> *()> &topology, unsigned int own_address,
                     const std::string &logger_name) {
    // Configure logging.
    _logger = spdlog::get(logger_name);
    _current_capabilities = new CurrentCapabilities(topology, own_address);
}
#else
Resources::Resources(const std::string& logger_name) {
    // Configure logging.
    _logger = spdlog::get(logger_name);
}
#endif