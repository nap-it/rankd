#include "structs/reservation.h"

Reservation::Reservation(const Reservation& reservation) {
    _state = ReservationState::CREATED;
    _priority = reservation.priority();
    _capabilities = reservation.requirements();
    _uuid = reservation.uuid();
    _listener_length = reservation.listener_length();
    _listener = reservation.listener();
}

Reservation::Reservation(const RequestingCapabilities& capabilities, uint8_t priority) {
    _state = ReservationState::CREATED;
    _priority = priority;
    _capabilities = capabilities;
}

void Reservation::add_next_node(const std::pair<std::vector<uint8_t>, IdentifierType> &node) {
    _next_nodes.push_back(node);
}

void Reservation::set_past_node(const std::pair<std::vector<uint8_t>, IdentifierType> &node) {
    _past_node = node;
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

std::array<uint8_t, RANK_LISTENER_MAX_LEN> Reservation::listener() const {
    return _listener;
}

UUIDv4 Reservation::uuid() const {
    return _uuid;
}

std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> Reservation::next_nodes() const {
    return _next_nodes;
}

std::pair<std::vector<uint8_t>, IdentifierType> Reservation::past_node() const {
    return _past_node;
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
    return _priority < reservation.priority() and _uuid < reservation.uuid();
}

bool Reservation::operator==(const Reservation& reservation) const {
    return _uuid == reservation.uuid() and _state == reservation.state() and
           _capabilities == reservation.requirements();
}

Reservation::~Reservation() {
}
