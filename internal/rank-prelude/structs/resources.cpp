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

float Resources::estimate_bid(const Reservation& reservation) const {
    // Phase 1: Bare-metal criterium for reservation.
    float node_resources = bare_metal_resource_assessment(reservation.requirements());

    // Phase 2: Current capability for performance of requirement list.
    float current_resources = current_resource_assessment(reservation.requirements());

    // Phase 3: Fairness.
    float fairness = reservation.priority() / RANK_MAX_PRIORITY;

    // Phase 4: Proximity criterium.
    float proximity = proximity_assessment(reservation.listener(), reservation.listener_length());

    // Phase 5: Historical Performance criterium.
    float hysteresis = hysteresis_assessment(reservation);

    return node_resources * (proximity + hysteresis) / 2 * current_resources * (fairness / RANK_MAX_PRIORITY);
}

float Resources::bare_metal_resource_assessment(const RequestingCapabilities &requirements) const {
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

float Resources::current_resource_assessment(const RequestingCapabilities &requirements) const {
    // For each requirement, individually-assess it and sum it at the end, by means of a function.
    std::vector<float> individual_assessments{};

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

    std::function<float(const std::vector<float>&)> alpha;
    alpha = [&](const std::vector<float>& requirements) -> float {
        if (requirements.size() == 1) {
            return requirements.front();
        }
        std::vector<float> requirements_subvector;
        std::copy(requirements.begin()+1, requirements.end(), std::back_inserter(requirements_subvector));
        return RANK_CURRENT_RESOURCES_EVAL_THRESHOLD*requirements.front() + (1-RANK_CURRENT_RESOURCES_EVAL_THRESHOLD)*alpha(requirements_subvector);
    };

    return alpha(individual_assessments);
}

float Resources::proximity_assessment(const std::array<uint8_t, 16> &target, uint8_t target_length) const {

    return 1.0;
}

float Resources::hysteresis_assessment(const Reservation &reservation) const {

    return 1.0;
}

Reservation* Resources::available_for_performance(const Reservation& statement, uint8_t priority) {
    // Check if currently there are resources to perform this statement.
    if (statement.requirements() < *_current_capabilities) {
        // If so, then simply accept it.
        return new Reservation(statement);
    } else {
        // Otherwise, then check if there is a reservation with lower priority that could be unconsidered.
        auto reservation_pointer = std::min_element(_reservations.begin(), _reservations.end());

        // If the list is empty, then the pointer will reach the end, returning nullptr as the requirements could not be
        // performed.
        if (reservation_pointer == _reservations.end()) {
            return nullptr;
        }

        // If the minimum priority is higher or equal than the requested one, terminate will nullptr.
        if (reservation_pointer->priority() >= statement.priority()) {
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
        throw std::exception();  // TODO
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