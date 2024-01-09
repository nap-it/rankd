#include "structs/resources.h"

Resources* Resources::get_instance() {
    static Resources* instance = new Resources();
    return instance;
}

float Resources::estimate_bid(const RequestingCapabilities& capabilities) const {
    // FIXME Talvez cortar este método?
}

float Resources::estimate_bid(const Reservation& reservation) const {
    float node_resources = 0;
    float proximity = 0;
    float hysteresis = 0;
    float current_resources = 0;
    float fairness = reservation.priority() / RANK_MAX_PRIORITY;

    return node_resources * (proximity + hysteresis)/2 * current_resources * (fairness/RANK_MAX_PRIORITY);
}

float Resources::estimate_current_resources_on_requirements(const RequestingCapabilities& requirements) const {

}

Reservation* Resources::available_for_performance(const Reservation& statement, uint8_t priority) {

}

Resources* Resources::replenish_reservation(Reservation* reservation) {
    // Find reservation in reservations and if no such reservation was found, then throw exception.
    if (!_reservations.contains(*reservation)) {
        throw std::exception(); // TODO
    }

    // Pass reservation state to replenish.
    reservation->replenish();

    // Remove reservation in the set of reservations.
    _reservations.erase(_reservations.find(*reservation));

    return this;
}

Resources* Resources::mark_reservation(Reservation* reservation) {
    // Find reservation in reservations and if no such reservation was found, then throw exception.
    if (!_reservations.contains(*reservation)) {
        throw std::exception(); // TODO
    }

    // Remove reservation in the set of reservations.
    _reservations.erase(_reservations.find(*reservation));

    // Mark reservation as reserved.
    reservation->mark_reserved();

    // Add the modified reservation to the set of reservations.
    _reservations.insert(*reservation);

    return this;}

Resources* Resources::mark_pre_reservation(Reservation* reservation) {
    // Find reservation in reservations and if no such reservation was found, then throw exception.
    if (!_reservations.contains(*reservation)) {
        throw std::exception(); // TODO
    }

    // Remove reservation in the set of reservations.
    _reservations.erase(_reservations.find(*reservation));

    // Mark reservation as pre-reserved.
    reservation->mark_pre_reserved();

    // Add the modified reservation to the set of reservations.
    _reservations.insert(*reservation);

    return this;
}

std::set<Reservation> Resources::reservations() const {
    return _reservations;
}

size_t Resources::reservations_size() const {
    return _reservations.size();
}

std::vector<std::vector<uint8_t>> Resources::get_connections_to(const std::string& target) const {}

unsigned int Resources::connections_cardinal(const std::vector<std::vector<uint8_t>>& connections) const {
    return connections.size();
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

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }
}

Resources::~Resources() {}

Resources::Resources() {}