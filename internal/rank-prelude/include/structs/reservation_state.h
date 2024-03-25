#ifndef RANK_PRELUDE_RESERVATION_STATE_H
#define RANK_PRELUDE_RESERVATION_STATE_H

enum class ReservationState { CREATED, VIRTUALLY_PRE_RESERVED, PRE_RESERVED, RESERVED, CLEARED };

inline const char* reservation_state_as_string(const ReservationState& state) {
    switch (state) {
        case ReservationState::CREATED:
            return "created";
        case ReservationState::VIRTUALLY_PRE_RESERVED:
            return "virtually pre-reserved";
        case ReservationState::PRE_RESERVED:
            return "pre-reserved";
        case ReservationState::RESERVED:
            return "reserved";
        case ReservationState::CLEARED:
            return "cleared";
    }

    return "";
}

#endif  // RANK_PRELUDE_RESERVATION_STATE_H
