#include "structs/reservation.h"

Reservation::Reservation(const Reservation& reservation, const std::string& logger_name) {
    _logger = spdlog::get(logger_name);
    _state = ReservationState::CREATED;
    _priority = reservation.priority();
    _capabilities = reservation.requirements();
    _uuid = reservation.uuid();
    _listener_length = reservation.listener_length();
    _listener = reservation.listener();

    _logger->debug("[Reservation] [{}] Reservation object created with state {}.", _uuid, reservation_state_as_string(_state));
}

Reservation::Reservation(const RequestingCapabilities& capabilities, uint8_t priority, const std::string& logger_name) {
    _logger = spdlog::get(logger_name);
    _state = ReservationState::CREATED;
    _priority = priority;
    _capabilities = capabilities;
}

void Reservation::add_next_node(const std::pair<std::vector<uint8_t>, IdentifierType> &node) {
    _next_nodes.push_back(node);

    _logger->debug("[Reservation] [{}] Updated a next node for this reservation: {} --> {}.", _uuid, (_past_node.first.empty() ? "{}" : past_node_as_string()), (_next_nodes.empty() ? "{}" : next_nodes_as_string()));
}

void Reservation::set_past_node(const std::pair<std::vector<uint8_t>, IdentifierType> &node) {
    _past_node = node;

    _logger->debug("[Reservation] [{}] Updated the past node for this reservation: {} --> {}.", _uuid, (_past_node.first.empty() ? "{}" : past_node_as_string()), (_next_nodes.empty() ? "{}" : next_nodes_as_string()));
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
    return this;
}

void Reservation::wait_for_sacrifice() {
    assert(_state == ReservationState::CREATED);

    _state = ReservationState::VIRTUALLY_PRE_RESERVED;

    _logger->debug("[Reservation] [{}] Reservation state changed from {} to {}.", _uuid, reservation_state_as_string(ReservationState::CREATED),
                   reservation_state_as_string(_state));
}

void Reservation::reserve() {
    assert(_state == ReservationState::CREATED or _state == ReservationState::VIRTUALLY_PRE_RESERVED);

    ReservationState old_state = _state;

    _state = ReservationState::RESERVED;

    _logger->debug("[Reservation] [{}] Reservation state changed from {} to {}.", _uuid, reservation_state_as_string(old_state),
                   reservation_state_as_string(_state));
}

void Reservation::pre_reserve() {
    assert(_state == ReservationState::CREATED);

    _state = ReservationState::PRE_RESERVED;

    _logger->debug("[Reservation] [{}] Reservation state changed from {} to {}.", _uuid, reservation_state_as_string(ReservationState::PRE_RESERVED),
                   reservation_state_as_string(_state));
}

void Reservation::unconsider() {
    assert(_state == ReservationState::CREATED);

    _state = ReservationState::CLEARED;

    _logger->debug("[Reservation] [{}] Reservation state changed from {} to {}.", _uuid, reservation_state_as_string(ReservationState::CLEARED),
                   reservation_state_as_string(_state));
}

void Reservation::replenish() {
    assert(_state == ReservationState::RESERVED or _state == ReservationState::PRE_RESERVED or
           _state == ReservationState::VIRTUALLY_PRE_RESERVED);

    ReservationState old_state = _state;

    _state = ReservationState::CLEARED;

    _logger->debug("[Reservation] [{}] Reservation state changed from {} to {}.", _uuid, reservation_state_as_string(old_state),
                   reservation_state_as_string(_state));
}

void Reservation::mark_pre_reserved() {
    assert(_state == ReservationState::RESERVED);

    _state = ReservationState::PRE_RESERVED;

    _logger->debug("[Reservation] [{}] Reservation state changed from {} to {}.", _uuid, reservation_state_as_string(ReservationState::PRE_RESERVED),
                   reservation_state_as_string(_state));
}

void Reservation::mark_reserved() {
    assert(_state == ReservationState::PRE_RESERVED);

    _state = ReservationState::RESERVED;

    _logger->debug("[Reservation] [{}] Reservation state changed from {} to {}.", _uuid, reservation_state_as_string(ReservationState::RESERVED),
                   reservation_state_as_string(_state));
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

std::string Reservation::next_nodes_as_string() const {
    std::stringstream to_return{};

    for (const auto& [node, type] : _next_nodes) {
        if (type == IdentifierType::MAC) {
            to_return << std::hex << std::setw(2) << std::setfill('0') << node.at(0) << ":"
                      << std::hex << std::setw(2) << std::setfill('0') << node.at(1) << ":"
                      << std::hex << std::setw(2) << std::setfill('0') << node.at(2) << ":"
                      << std::hex << std::setw(2) << std::setfill('0') << node.at(3) << ":"
                      << std::hex << std::setw(2) << std::setfill('0') << node.at(4) << ":"
                      << std::hex << std::setw(2) << std::setfill('0') << node.at(5) << ", ";
        } else if (type == IdentifierType::IPv4) {
            to_return << node.at(0) << "." << node.at(1) << "." << node.at(2) << "." << node.at(3) << ", ";
        } else if (type == IdentifierType::IPv6) {
            for (int i = 0; i != 16; i++) {
                to_return << std::hex << std::setw(2) << std::setfill('0') << node.at(i);
                if (i % 2 != 0) {
                    to_return << ":";
                }
            }
            to_return << ", ";
        } else if (type == IdentifierType::DDS) {
            for (int i = 0; i != 16; i++) {
                to_return << std::hex << std::setfill('0') << node.at(i);
            }
            to_return << ", ";
        } else if (type == IdentifierType::Simulation) {
            to_return << ((node.at(1) << 8) | node.at(0)) << ", ";
        }
    }

    return "(" + to_return.str().substr(0, to_return.str().size()-2) + ")";
}

std::string Reservation::past_node_as_string() const {
    std::stringstream to_return;

    IdentifierType type = _past_node.second;
    std::vector<uint8_t> node = _past_node.first;

    if (type == IdentifierType::MAC) {
        to_return << std::hex << std::setw(2) << std::setfill('0') << node.at(0) << ":"
                  << std::hex << std::setw(2) << std::setfill('0') << node.at(1) << ":"
                  << std::hex << std::setw(2) << std::setfill('0') << node.at(2) << ":"
                  << std::hex << std::setw(2) << std::setfill('0') << node.at(3) << ":"
                  << std::hex << std::setw(2) << std::setfill('0') << node.at(4) << ":"
                  << std::hex << std::setw(2) << std::setfill('0') << node.at(5) << ", ";
    } else if (type == IdentifierType::IPv4) {
        to_return << node.at(0) << "." << node.at(1) << "." << node.at(2) << "." << node.at(3) << ", ";
    } else if (type == IdentifierType::IPv6) {
        for (int i = 0; i != 16; i++) {
            to_return << std::hex << std::setw(2) << std::setfill('0') << node.at(i);
            if (i % 2 != 0) {
                to_return << ":";
            }
        }
        to_return << ", ";
    } else if (type == IdentifierType::DDS) {
        for (int i = 0; i != 16; i++) {
            to_return << std::hex << std::setfill('0') << node.at(i);
        }
        to_return << ", ";
    } else if (type == IdentifierType::Simulation) {
        to_return << ((node.at(1) << 8) | node.at(0)) << ", ";
    }

    return to_return.str().substr(0, to_return.str().size()-2);
}