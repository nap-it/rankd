#include "structs/reservation.h"

Reservation::Reservation(const Reservation& reservation) {
}

Reservation::Reservation(const RequestingCapabilities& capabilities, uint8_t priority) {
}

uint8_t Reservation::priority() const {
    return _priority;
}

RequestingCapabilities Reservation::requirements() const {
    return _capabilities;
}

ReservationState Reservation::state() const {
    return _state;
}

uint8_t Reservation::listener_length() const {
    return _listener_length;
}

uint8_t* Reservation::listener() const {
    return _listener;
}

UUIDv4 Reservation::uuid() const {
    return _uuid;
}

Reservation* Reservation::mark_listener(const std::vector<uint8_t>& listener) {
}

void Reservation::wait_for_sacrifice() {
    assert(_state == ReservationState::CREATED);

    _state = ReservationState::VIRTUALLY_PRE_RESERVED;
}

void Reservation::reserve() {
    assert(_state == ReservationState::CREATED or _state == ReservationState::VIRTUALLY_PRE_RESERVED);

    _state = ReservationState::RESERVED;
}

void Reservation::pre_reserve() {
    assert(_state == ReservationState::CREATED);

    _state = ReservationState::PRE_RESERVED;
}

void Reservation::unconsider() {
    assert(_state == ReservationState::CREATED);

    _state = ReservationState::CLEARED;
}

void Reservation::replenish() {
    assert(_state == ReservationState::RESERVED or _state == ReservationState::PRE_RESERVED or
           _state == ReservationState::VIRTUALLY_PRE_RESERVED);

    _state = ReservationState::CLEARED;
}

void Reservation::mark_pre_reserved() {
    assert(_state == ReservationState::RESERVED);

    _state = ReservationState::PRE_RESERVED;
}

void Reservation::mark_reserved() {
    assert(_state == ReservationState::PRE_RESERVED);

    _state = ReservationState::RESERVED;
}

bool Reservation::operator<(const Reservation& reservation) const {
}

bool Reservation::operator==(const Reservation& reservation) const {
    return _uuid == reservation.uuid() and _state == reservation.state() and
           _capabilities == reservation.requirements();
}

Reservation::~Reservation() {
}
