#ifndef RANK_PRELUDE_RESERVATION_H
#define RANK_PRELUDE_RESERVATION_H

#include "structs/identifier.h"
#include "structs/requesting_capabilities.h"
#include "structs/reservation_state.h"

class Reservation {
public:
    // Instance handling.
    Reservation(const RequestingCapabilities& capabilities, uint8_t priority);

    // Getters.
    uint8_t priority() const;
    RequestingCapabilities capabilities() const;
    ReservationState state() const;

    // State changers.
    void wait_for_sacrifice();
    void reserve();
    void pre_reserve();
    void unconsider();
    void replenish();
    void mark_pre_reserved();
    void mark_reserved();
    void mark_virtually_pre_reserved();

    // Operators.
    bool operator<(const Reservation& reservation);
private:
    ReservationState _state;
    uint8_t _priority;
    RequestingCapabilities _capabilities;
    UUIDv4 _uuid;
};


#endif  // RANK_PRELUDE_RESERVATION_H
