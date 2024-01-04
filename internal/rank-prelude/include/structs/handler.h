#ifndef RANK_PRELUDE_HANDLER_H
#define RANK_PRELUDE_HANDLER_H

#include <array>
#include <mutex>
#include <thread>

#include "structs/bidset.h"
#include "structs/handler_state.h"
#include "structs/identifier.h"
#include "structs/message.h"
#include "structs/requesting_capabilities.h"
#include "structs/reservation.h"
#include "structs/resources.h"
#include "structs/timeout_handler_spec.h"
#include "structs/translation_table.h"

class Handler {
public:
    // Instance handling.
    Handler();

    // Handling actions.
    Handler* handle(const Message& message);
    bool create_translation(const UUIDv4& to, const UUIDv4& from) const;
    const UUIDv4& locate_original_of(const UUIDv4& translated) const;
    bool is_translation_table_empty_for(const UUIDv4& uuid) const;
    void new_bid(float bid, std::array<uint8_t, 4>& ipv4_address);
    void new_bid(float bid, std::array<uint8_t, 6>& mac_address);
    void new_bid(float bid, std::array<uint8_t, 16>& ipv6_address);
    void clear_bids();
    size_t cardinal_bids();
    std::set<std::string> min_bids() const;
    bool is_min_bid_unique(const std::set<std::string>& targets) const;

    // Transformation tools.
    Reservation produce_reservation(const std::vector<RequestingCapabilities>& capabilities) const;

    // Getters.
    UUIDv4 id() const;
    HandlerState state() const;
    Reservation* associated_reservation() const;

    // Static tools.
    static bool is_me(std::array<uint8_t, 4>& ipv4_address);
    static bool is_me(std::array<uint8_t, 6>& mac_address);
    static bool is_me(std::array<uint8_t, 16>& ipv6_address);

    // Threading control mechanisms.
    Handler* execute();
    Handler* stop();
    bool is_running() const;

protected:
    void operator()();

private:
    UUIDv4 _uuid;
    Message _message;
    HandlerState _state;
    BidSet _bids;
    std::mutex _bids_locker;
    Resources* _resources;
    Reservation* _reservation;
    TimeoutHandlerSpec* _timeout_spec;
    TranslationTable* _translation_table;
    bool _running = false;
    unsigned int _waiting_time = 1000;
    std::thread _thread;
};


#endif  // RANK_PRELUDE_HANDLER_H
