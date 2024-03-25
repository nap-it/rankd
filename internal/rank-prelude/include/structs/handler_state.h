#ifndef RANK_PRELUDE_HANDLER_STATE_H
#define RANK_PRELUDE_HANDLER_STATE_H

enum class HandlerState {
    READY,
    PRESENTING,
    AUCTION_BIDDING,
    AUCTION_WAITING,
    ASSESSING,
    PRE_RESERVED,
    RESERVED,
    REPLENISHING,
    CLOSED
};

inline const char* handler_state_to_string(const HandlerState& state) {
    switch (state) {
        case HandlerState::READY:
            return "ready";
        case HandlerState::PRESENTING:
            return "presenting";
        case HandlerState::AUCTION_BIDDING:
            return "auction bidding";
        case HandlerState::AUCTION_WAITING:
            return "auction waiting";
        case HandlerState::ASSESSING:
            return "assessing";
        case HandlerState::PRE_RESERVED:
            return "pre-reserved";
        case HandlerState::RESERVED:
            return "reserved";
        case HandlerState::REPLENISHING:
            return "replenishing";
        case HandlerState::CLOSED:
            return "closed";
    }

    return "";
}

#endif  // RANK_PRELUDE_HANDLER_STATE_H
