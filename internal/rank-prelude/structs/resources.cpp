#include "structs/resources.h"

Resources* Resources::get_instance() {
    static Resources* instance = new Resources();
    return instance;
}

float Resources::estimate_bid(const RequestingCapabilities& capabilities) const {
    // FIXME Talvez cortar este método?
}

float Resources::estimate_bid(const Reservation& reservation) const {
    float node_resources = 0;     // TODO
    float proximity = 0;          // TODO
    float hysteresis = 0;         // TODO
    float current_resources = 0;  // TODO
    float fairness = reservation.priority() / RANK_MAX_PRIORITY;

    return node_resources * (proximity + hysteresis) / 2 * current_resources * (fairness / RANK_MAX_PRIORITY);
}

Reservation* Resources::available_for_performance(const Reservation& statement, uint8_t priority) {
    // Check if currently there are resources to perform this statement.
    if (statement.requirements() < _current_capabilities) {
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
        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }
}

Resources::~Resources() {
}

Resources::Resources() {
}