#ifndef RANK_PRELUDE_HANDLER_H
#define RANK_PRELUDE_HANDLER_H

#include <array>
#include <chrono>
#include <mutex>
#include <thread>

#include "spdlog/spdlog.h"

#include "constants.h"

#include "structs/bidset.h"
#include "structs/store.h"
#include "structs/translation_table.h"

#include "utils/messaging.h"
#include "utils/neighboring.h"

class TimeoutHandler;

#include "structs/dispatcher.h"
#include "structs/handler_state.h"
#include "structs/identifier.h"
#include "structs/message.h"
#include "structs/messages/all.h"
#include "structs/requesting_capabilities.h"
#include "structs/reservation.h"
#include "structs/resources.h"
#include "structs/timeout_handler.h"

#include "utils/identity.h"

class Handler {
public:
    // Instance handling.
    Handler(Resources* resources, TranslationTable* translation_table, std::mutex* translation_table_mutex,
            TimeoutHandler* timeout_handler, Store* store, std::mutex* store_mutex, const UUIDv4& uuid);
    Handler(Resources* resources, TranslationTable* translation_table, std::mutex* translation_table_mutex,
            TimeoutHandler* timeout_handler, Store* store, std::mutex* store_mutex, const Header& header);

    // Handling actions.
    Handler* handle(Message* message);
    UUIDv4 create_translation(const UUIDv4& original);
    UUIDv4 locate_original_of(const UUIDv4& translated);
    bool is_translation_table_empty_for(const UUIDv4& uuid);
    bool is_translation_table_empty();
    void new_bid(float bid, std::array<uint8_t, 4>& ipv4_address);
    void new_bid(float bid, std::array<uint8_t, 6>& mac_address);
    void new_bid(float bid, std::array<uint8_t, 16>& ipv6_address);
    void new_bid(float bid, std::string& dds_address);
    // TODO Define new bid for simulation.
    void clear_bids();
    size_t cardinal_bids();
    std::set<std::pair<std::vector<uint8_t>, IdentifierType>> min_bids();
    bool is_min_bid_unique(const std::set<std::pair<std::vector<uint8_t>, IdentifierType>>& targets) const;
    bool is_bid_in_store(const UUIDv4& id) const;   // From process to handler.
    bool is_uuid_in_store(const UUIDv4& id) const;  // From process to handler.

    // Transformation tools.
    void produce_reservation(const RequestingCapabilities& capabilities, uint8_t priority, const std::vector<uint8_t>& listener);

    // Getters.
    UUIDv4 id() const;
    HandlerState state() const;
    Reservation* associated_reservation() const;

    // Dispatcher configurations.
    Handler* borrow(Dispatcher* dispatcher);

    // Source and accepting node identifier setter.
    Handler* mark_source(const std::pair<std::vector<uint8_t>, IdentifierType>& source);
    Handler* mark_accepting_node(const std::pair<std::vector<uint8_t>, IdentifierType>& node);

    // Threading control mechanisms.
    Handler* execute();
    Handler* stop();
    bool is_running() const;
    void operator()();

private:
    UUIDv4 _uuid;
    Dispatcher* _dispatcher = nullptr;
    std::pair<std::vector<uint8_t>, IdentifierType> _source_identifier{};
    std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> _accepting_nodes{};
    Message* _message = nullptr;
    HandlerState _state;
    BidSet _bids;
    std::mutex _bids_locker;
    Resources* _resources = nullptr;
    Reservation* _reservation = nullptr;
    Store* _store = nullptr;
    std::mutex* _store_locker = nullptr;
    TimeoutHandler* _timeout_handler = nullptr;
    TranslationTable* _translation_table = nullptr;
    std::mutex* _translation_table_locker = nullptr;
    bool _running = false;
    unsigned int _waiting_time = 1000;
    std::thread _thread;
    std::shared_ptr<spdlog::logger> _logger = spdlog::get("rank-logger");
};


#endif  // RANK_PRELUDE_HANDLER_H
