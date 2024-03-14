#ifndef RANK_PRELUDE_RESERVATION_H
#define RANK_PRELUDE_RESERVATION_H

#include <cassert>
#include <cstdint>

#include "structs/identifier.h"
#include "structs/requesting_capabilities.h"
#include "structs/reservation_state.h"

class Reservation {
public:
    // Instance handling.
    Reservation(const Reservation& reservation);
    Reservation(const RequestingCapabilities& capabilities, uint8_t priority);

    // Past and next nodes handling.
    void add_next_node(const std::pair<std::vector<uint8_t>, IdentifierType>& node);
    void set_past_node(const std::pair<std::vector<uint8_t>, IdentifierType>& node);

    // Getters.
    uint8_t priority() const;
    RequestingCapabilities requirements() const;
    ReservationState state() const;
    uint8_t listener_length() const;
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener() const;
    UUIDv4 uuid() const;
    std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> next_nodes() const;
    std::pair<std::vector<uint8_t>, IdentifierType> past_node() const;

    // Setters.
    Reservation* mark_listener(const std::vector<uint8_t>& listener);

    // State changers.
    void wait_for_sacrifice();
    void reserve();
    void pre_reserve();
    void unconsider();
    void replenish();
    void mark_pre_reserved();
    void mark_reserved();

    // Operators.
    bool operator<(const Reservation& reservation) const;
    bool operator==(const Reservation& reservation) const;

    // Destructor.
    ~Reservation();

private:
    ReservationState _state;
    uint8_t _priority;
    RequestingCapabilities _capabilities;
    UUIDv4 _uuid{};
    uint8_t _listener_length{};
    std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> _next_nodes{};
    std::pair<std::vector<uint8_t>, IdentifierType> _past_node{};
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> _listener{};
};


#endif  // RANK_PRELUDE_RESERVATION_H
