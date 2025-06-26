#include "structs/handler.h"

Handler::Handler(Resources *resources, TranslationTable *translation_table, std::mutex *translation_table_mutex,
                 TimeoutHandler *timeout_handler, Store *store, std::mutex *store_mutex, const UUIDv4 &uuid,
                 const std::string &logger_name) {
    // Update logging mechanism.
    _logger = spdlog::get(logger_name);
    _logger->trace("[Handler] [{}] Creating handler for this UUID...", display(uuid));

    // Update all attributes from caller.
    _uuid = uuid;
    _bids = {};

    // Update inner structures from caller.
    _resources = resources;
    _store = store;
    _store_locker = store_mutex;
    _translation_table = translation_table;
    _translation_table_locker = translation_table_mutex;
    _timeout_handler = timeout_handler;

    // Initialize other attributes.
    _reservation = nullptr;

    // Change handler's state to Ready.
    _state = HandlerState::READY;
    _logger->trace("[Handler] [{}] The handler was successfully created and given the state of {}.", display(uuid),
                   handler_state_to_string(_state));
    _logger->info("A handler was created for the {} session.", display(uuid));
}

Handler::Handler(Resources *resources, TranslationTable *translation_table, std::mutex *translation_table_mutex,
                 TimeoutHandler *timeout_handler, Store *store, std::mutex *store_mutex, const Header &header,
                 const std::string &logger_name) {
    // Update logging mechanism.
    _logger = spdlog::get(logger_name);

    // Update all attributes from caller.
    _uuid = header.uuid();
    _bids = {};

    // Update inner structures from caller.
    _resources = resources;
    _store = store;
    _store_locker = store_mutex;
    _translation_table = translation_table;
    _translation_table_locker = translation_table_mutex;
    _timeout_handler = timeout_handler;

    // Initialize other attributes.
    _reservation = nullptr;

    // Change handler's state to Ready.
    _state = HandlerState::READY;
}

Handler *Handler::handle(Message *message) {
    // Update the message pointer.
    _message = message;

    return this;
}

UUIDv4 Handler::create_translation(const UUIDv4 &original) {
    // Generate a new UUID with the timestamp of the original.
    auto original_vector = marshall_into_vector(original);
    auto timestamp_slice = std::vector<uint8_t>(original_vector.begin(), original_vector.begin()+6);
    uint64_t timestamp;
    for (int i = 0; i < 6; ++i) {
        timestamp = (timestamp << 8) | timestamp_slice[i];
    }
    UUIDv4 new_uuid = generate_new_uuid_with((long) timestamp);

    // Save translation in the TranslationTable.
    {
        std::lock_guard<std::mutex> lock(*_translation_table_locker);
        _translation_table->insert({new_uuid, original});
    }

    // // Save translation in Store.
    // {
    //     std::lock_guard<std::mutex> lock(*_store_locker);
    //     _store->insert({new_uuid, this});
    // }

    // Return the new UUID.
    return new_uuid;
}

UUIDv4 Handler::locate_original_of(const UUIDv4 &translated) {
    // If there is no such thing as the UUID in translated, in TranslationTable, return 0.
    if (is_translation_table_empty_for(translated)) {
        return 0;
    }

    // Otherwise, safely return the value in the TranslationTable.
    std::lock_guard<std::mutex> lock(*_translation_table_locker);
    return _translation_table->at(translated);
}

bool Handler::is_translation_table_empty_for(const UUIDv4 &uuid) {
    std::lock_guard<std::mutex> lock(*_translation_table_locker);
    return not _translation_table->contains(uuid);
}

bool Handler::is_translation_table_empty() {
    std::lock_guard<std::mutex> lock(*_translation_table_locker);
    return _translation_table->empty();
}

void Handler::new_bid(float bid, uint8_t simuzilla_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(
            std::make_pair(bid, std::make_pair(std::vector<uint8_t>({simuzilla_address}), IdentifierType::Simulation)));
    _arriving_bids += 1;
}

void Handler::new_bid(float bid, std::array<uint8_t, 4> &ipv4_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::make_pair(std::vector<uint8_t>(ipv4_address.begin(), ipv4_address.end()),
                                                    IdentifierType::IPv4)));
    _arriving_bids += 1;
}

void Handler::new_bid(float bid, std::array<uint8_t, 6> &mac_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::make_pair(std::vector<uint8_t>(mac_address.begin(), mac_address.end()),
                                                    IdentifierType::MAC)));
    _arriving_bids += 1;
}

void Handler::new_bid(float bid, std::array<uint8_t, 16> &ipv6_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::make_pair(std::vector<uint8_t>(ipv6_address.begin(), ipv6_address.end()),
                                                    IdentifierType::IPv6)));
    _arriving_bids += 1;
}

void Handler::new_bid(float bid, std::string &dds_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::make_pair(std::vector<uint8_t>(dds_address.begin(), dds_address.end()),
                                                    IdentifierType::DDS)));
    _arriving_bids += 1;
}

void Handler::clear_bids() {
    _bids.clear();
    _arriving_bids = 0;
    _waiting_bids = 0;
}

size_t Handler::cardinal_bids() {
    return _bids.size();
}

std::set<std::pair<std::vector<uint8_t>, IdentifierType>> Handler::max_bids() {
    // Copy the bids set to a safe-to-modify variable.
    BidSet bids_copy{};
    {
        std::lock_guard<std::mutex> lock(_bids_locker);
        bids_copy = _bids;
    }

    // Assume the first item in reverse bids is the maximum one.
    float maximum_bid = bids_copy.rbegin()->first;

    _logger->trace("[Handler] [{}] The maximum bid obtained was of {}.", display(_uuid), maximum_bid);

    // Create the set to be returned.
    std::vector<std::pair<float, std::pair<std::vector<uint8_t>, IdentifierType>>> bids_copy_subset;
    std::set<std::pair<std::vector<uint8_t>, IdentifierType>> maximum_bids{};

    // Iterate over all the given bids and attempt to find smaller than current minimum and all equal bids.
    for (const auto &bid_item: bids_copy) {
        if (maximum_bid == bid_item.first) {
            maximum_bids.insert(bid_item.second);
        }
    }

    // Return the minimum bids new targets set.
    return maximum_bids;
}

bool Handler::is_max_bid_unique(const std::set<std::pair<std::vector<uint8_t>, IdentifierType>> &targets) const {
    return targets.size() == 1;
}

bool Handler::is_bid_in_store(const UUIDv4 &id) const {
    // If the store does not have the UUID, return false; otherwise check if pre-reserved.
    if (!is_uuid_in_store(id)) {
        return false;
    } else {
        return _store->at(id)->state() == HandlerState::PRE_RESERVED;
    }
}

bool Handler::is_uuid_in_store(const UUIDv4 &id) const {
    return _store->contains(id);
}

void Handler::produce_reservation(const RequestingCapabilities &capabilities, uint8_t priority,
                                  const std::vector<uint8_t> &listener) {
    // Create a new reservation object via a pointer.
    _reservation = new Reservation(capabilities, priority, _logger->name());

    // Mark listener as the listener of this reservation.
    _reservation->mark_listener(listener, listener.size());
}

UUIDv4 Handler::id() const {
    return _uuid;
}

HandlerState Handler::state() const {
    return _state;
}

Reservation *Handler::associated_reservation() const {
    return _reservation;
}

Handler *Handler::new_id(const UUIDv4 &id) {
    _uuid = id;

    return this;
}

Handler *Handler::register_api_methods(const std::function<bool(const UUIDv4 &)> &am_i_origin_for,
                                       const std::function<bool(const UUIDv4 &)> &remove_as_origin) {
    _am_i_origin_for = am_i_origin_for;
    _remove_as_origin = remove_as_origin;

    return this;
}

Handler *Handler::borrow(Dispatcher *dispatcher) {
    _dispatcher = dispatcher;

    return this;
}

bool Handler::all_bids_arrived() {
    std::lock_guard<std::mutex> guard(_bids_locker);

    _logger->trace("[TimeoutHandler] [Handler] [{}] Checking if all bids already arrived. {}", display(_uuid),
                   (_waiting_bids == _arriving_bids) ? "Yes." : "No.");

    if (_waiting_bids == _arriving_bids) {
        _auction_mutex.unlock();
        _auction_wait.notify_one();

        return true;
    } else {
        return false;
    }
}

void Handler::on_auct_timeout() {
    _logger->trace("[TimeoutHandler] [Handler] [{}] Timeout for AUCT was reached.", display(_uuid));

    // Check if timeout is needed.
    if (all_bids_arrived()) {
        _logger->trace(
                "[TimeoutHandler] [Handler] [{}] Timeout for AUCT was not needed, since all bids arrived on time.",
                display(_uuid));

        return;
    }

    // Stop new bids from being considered.
    _auction_mutex.unlock();
    _auction_wait.notify_one();
    _logger->trace("[TimeoutHandler] [Handler] [{}] The auction was stopped.", display(_uuid));
}

void Handler::on_bid_timeout() {
    _logger->trace("[TimeoutHandler] [Handler] [{}] Timeout for BID was reached.", display(_uuid));
    if (_state != HandlerState::CLOSED and
        _state != HandlerState::RESERVED or (_message == nullptr and _state == HandlerState::PRE_RESERVED)) {
        // Delete bids from the Store with this handler's UUID.
        _bids.clear();

        // Delete pre-reservations with this handler's UUID.
        _resources->replenish_reservation(_reservation);

        // Mark this handler's state as CLOSED.
        _state = HandlerState::CLOSED;
    }
}

void Handler::on_ear_timeout() {
    _logger->trace("[TimeoutHandler] [Handler] [{}] Timeout for EAR was reached.", display(_uuid));
    if (_state != HandlerState::CLOSED and
        _state != HandlerState::RESERVED or (_message == nullptr and _state == HandlerState::PRE_RESERVED)) {
        // Delete bids from the Store with this handler's UUID.
        _bids.clear();

        // Delete pre-reservations with this handler's UUID.
        _resources->replenish_reservation(_reservation);

        // Mark this handler's state as CLOSED.
        _state = HandlerState::CLOSED;
    }
}

void Handler::on_mar_timeout() {
    _logger->trace("[TimeoutHandler] [Handler] [{}] Timeout for MAR was reached.", display(_uuid));
    if (_state != HandlerState::CLOSED and
        _state != HandlerState::RESERVED or (_message == nullptr and _state == HandlerState::PRE_RESERVED)) {
        // Delete bids from the Store with this handler's UUID.
        _bids.clear();

        // Delete pre-reservations with this handler's UUID.
        _resources->replenish_reservation(_reservation);

        // Mark this handler's state as CLOSED.
        _state = HandlerState::CLOSED;
    }
}

void Handler::on_rep_timeout() {
    _logger->trace("[TimeoutHandler] [Handler] [{}] Timeout for REP was reached.", display(_uuid));
    if (_state != HandlerState::CLOSED or (_message == nullptr and _state == HandlerState::REPLENISHING)) {
        // Remove reservations with this handler's UUID.
        _resources->replenish_reservation(_reservation);

        // Mark this handler's state as CLOSED.
        _state = HandlerState::CLOSED;
    }
}

#ifdef FROM_SIMUZILLA

Handler *Handler::borrow(std::function<std::vector<std::pair<uint8_t, uint8_t>>(uint8_t)> function) {
    _get_connections_to = std::move(function);

    return this;
}

Handler *Handler::borrow(std::function<bool(uint8_t)> function) {
    _is_me = std::move(function);

    return this;
}

#endif

Handler *Handler::mark_source(const std::pair<std::vector<uint8_t>, IdentifierType> &source) {
    _source_identifier = source;

    return this;
}

Handler *Handler::mark_accepting_node(const std::pair<std::vector<uint8_t>, IdentifierType> &node) {
    _accepting_nodes.push_back(node);

    return this;
}

Handler *Handler::execute() {
    if (_running) {
        return this;
    }

    if (_thread.joinable()) {
        _thread.join();
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

Handler *Handler::stop() {
    if (!_running) {
        return this;
    }

    _running = false;
    //_thread.join();

    return this;
}

bool Handler::is_running() const {
    return _running;
}

void Handler::operator()() {
    _logger->trace("[Handler] [{}] This handler session is starting/resuming functions.", display(_uuid));

    // Variables for debugging purposes.
    HandlerState old_state{};

    // Initialization for the threading mechanism.
    while (_running) {
        if (_message != nullptr) {
            // Get reservation for this current UUID.
            _reservation = _resources->get_reservation_for(_uuid);

            if (_reservation != nullptr) {
                switch (_reservation->state()) {
                    case ReservationState::CREATED:
                    case ReservationState::VIRTUALLY_PRE_RESERVED:
                    case ReservationState::PRE_RESERVED:
                    case ReservationState::RESERVED:
                    case ReservationState::CLEARED:
                        if (_reservation->listener_length() != RANK_EAR_MESSAGE_LEN_LT_CODE_0 and
                            _reservation->listener_length() != RANK_EAR_MESSAGE_LEN_LT_CODE_1 and
                            _reservation->listener_length() != RANK_EAR_MESSAGE_LEN_LT_DDS and
                            _reservation->listener_length() != RANK_EAR_MESSAGE_LEN_LT_IP6 and
                            _reservation->listener_length() != RANK_EAR_MESSAGE_LEN_LT_MAC and
                            _reservation->listener_length() != RANK_EAR_MESSAGE_LEN_LT_IP4 and
                            _reservation->listener_length() != RANK_EAR_MESSAGE_LEN_LT_CODE_6 and
                            _reservation->listener_length() != RANK_EAR_MESSAGE_LEN_LT_CODE_7) {
                            _logger->error("[Handler] [{}] The reservation has an invalid listener length. Dropping...",
                                           display(_uuid));
                            throw std::exception();
                        }
                        break;
                    default:
                        _logger->error("[Handler] [{}] The reservation is in a invalid state. Dropping...",
                                       display(_uuid));
                        throw std::exception();
                }
            }

            // Parse message to their types and handle the parsing algorithm.
            switch (_message->type()) {
                case MessageType::EAR: {
                    _logger->trace("[Handler] [{}] The received message is an EAR message.", display(_uuid));
                    auto ear_message = dynamic_cast<EAR *>(_message);

                    // As this node is receiving a new EAR message, then change state to ASSESSING.
                    if (_state != HandlerState::PRE_RESERVED) {
                        old_state = _state;
                        _state = HandlerState::ASSESSING;
                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                       handler_state_to_string(old_state),
                                       handler_state_to_string(_state));
                    }

                    // (B.1) Check if the current node is the message's listener.
                    auto listener_field = ear_message->listener();
                    auto listener_field_length = ear_message->listener_length();
                    bool i_am_listener = false;
                    _logger->debug("[Process] [{}] (A.4) What is this message type? EAR.", display(_uuid));
                    switch (listener_field_length) {
#ifdef FROM_SIMUZILLA
                        case RANK_MAR_MESSAGE_LEN_LT_CODE_0: {
                            uint8_t simuzilla_address = listener_field.at(0);
                            _logger->trace("[Handler] [{}] Testing if I am id {}.", display(_uuid), simuzilla_address);
                            i_am_listener = simulated_is_me(simuzilla_address);

                            // Update inner reservation with the requirements of the message.
                            try {
                                if (_state != HandlerState::PRE_RESERVED) {
                                    produce_reservation(ear_message->requirements(), ear_message->priority(),
                                                        std::vector<uint8_t>({simuzilla_address}));
                                }
                            } catch (const std::invalid_argument &ia) {
                                _logger->error(
                                        "[Handler] [{}] Received message has non-compliant list of requirements. Ignoring messages and deleting handler.",
                                        display(_uuid));
                                old_state = _state;
                                _state = HandlerState::CLOSED;
                                stop();
                                return;
                            }
                        }
                            break;
#endif
                        case RANK_EAR_MESSAGE_LEN_LT_IP4: {
                            std::array<uint8_t, 4> ip4_address{};
                            for (int byte = 0; byte != 4; byte++) {
                                ip4_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(ip4_address);

                            // Update inner reservation with the requirements of the message.
                            if (_state != HandlerState::PRE_RESERVED) {
                                produce_reservation(ear_message->requirements(), ear_message->priority(),
                                                    std::vector<uint8_t>(ip4_address.begin(), ip4_address.end()));
                            }
                        }
                            break;
                        case RANK_EAR_MESSAGE_LEN_LT_MAC: {
                            std::array<uint8_t, 6> mac_address{};
                            for (int byte = 0; byte != 6; byte++) {
                                mac_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(mac_address);

                            // Update inner reservation with the requirements of the message.
                            if (_state != HandlerState::PRE_RESERVED) {
                                produce_reservation(ear_message->requirements(), ear_message->priority(),
                                                    std::vector<uint8_t>(mac_address.begin(), mac_address.end()));
                            }
                        }
                            break;
                        case RANK_EAR_MESSAGE_LEN_LT_IP6: {
                            std::array<uint8_t, 16> ip6_address{};
                            for (int byte = 0; byte != 16; byte++) {
                                ip6_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(ip6_address);

                            // Update inner reservation with the requirements of the message.
                            if (_state != HandlerState::PRE_RESERVED) {
                                produce_reservation(ear_message->requirements(), ear_message->priority(),
                                                    std::vector<uint8_t>(ip6_address.begin(), ip6_address.end()));
                            }
                        }
                            break;
                        default:
                            throw std::exception();  // TODO
                    }

                    // Mark reservation UUID.
                    if (_reservation != nullptr) {
                        _reservation->set_uuid(_uuid);
                    }

                    if (i_am_listener) {
                        _logger->debug("[Handler] [{}] (B.1) Am I the listener? Yes.", display(_uuid));

                        // (B.1.1.1) Can R be performed with priority p?
                        auto *position = _resources->available_for_performance(_reservation, ear_message->priority());
                        if (position != nullptr) {
                            _reservation = position;

                            _logger->debug("[Handler] [{}] (B.1.1.1) Can R be performed with priority p? Yes.",
                                           display(_uuid));

                            // (B.1.1.1.1.1) Reserve R with UUID in Store.
                            _resources->mark_reservation(_reservation);
                            _logger->debug("[Handler] [{}] (B.1.1.1.1.1) Reserve R with UUID in Store.",
                                           display(_uuid));

                            // (B.1.1.1.1.2) Create an ACC message and send it.
                            ACC *acc_message = new ACC(_uuid);
#ifdef FROM_SIMUZILLA
                            _logger->debug("[Simuzilla] [{}] (B.1.1.1.1.2) Starting.", display(_uuid));
                            std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                    get_connections_to(_source_identifier.first.at(0));
                            _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                           display(_uuid), connections_to_target_raw.size(),
                                           connections_to_target_raw.size() == 1 ? "" : "s",
                                           _source_identifier.first.at(0));
                            for (const auto &[connection, type]: connections_to_target_raw) {
                                _logger->trace("[Handler]                               |-> {} with depth {}",
                                               connection.at(0).second, connection.at(0).first);
                            }
                            std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target{};
                            uint8_t min_depth = UINT8_MAX;
                            for (const auto &[locators, type]: connections_to_target_raw) {
                                for (const auto &[depth, locator]: locators) {
                                    if (depth < min_depth) {
                                        min_depth = depth;
                                    }
                                    if (depth == min_depth) {
                                        connections_to_target.push_back({{locator}, type});
                                    }
                                }
                            }
                            _logger->debug("[Simuzilla] [{}] (B.1.1.1.1.2) Ending.", display(_uuid));
#else
                            std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target =
                                        get_connections_to(_source_identifier.first);
#endif
                            _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                           acc_message->display());
                            _dispatcher->send_message(acc_message, connections_to_target.front().first,
                                                      _source_identifier.second);
                            _logger->debug("[Handler] [{}] (B.1.1.1.1.2) Create an ACC message and send it back.",
                                           display(_uuid));

                            old_state = _state;
                            _state = HandlerState::RESERVED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (B.1.1.1.1.3) Change state to RESERVED and terminate thread.
                            stop();
                            _logger->debug("[Handler] [{}] (B.1.1.1.1.3) Terminate thread.", display(_uuid));
                            break;
                        } else {
                            _logger->debug("[Handler] [{}] (B.1.1.1) Can R be performed with priority p? No.",
                                           display(_uuid));

                            // (B.1.1.2) Create a REF message and send it.
                            REF *ref_message = new REF(_uuid);
#ifdef FROM_SIMUZILLA
                            _logger->debug("[Simuzilla] [{}] (B.1.1.2) Starting.", display(_uuid));
                            std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                    get_connections_to(_source_identifier.first.at(0));
                            _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                           display(_uuid), connections_to_target_raw.size(),
                                           connections_to_target_raw.size() == 1 ? "" : "s",
                                           _source_identifier.first.at(0));
                            for (const auto &[connection, type]: connections_to_target_raw) {
                                _logger->trace("[Handler]                               |-> {} with depth {}",
                                               connection.at(0).second, connection.at(0).first);
                            }
                            std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target{};
                            uint8_t min_depth = UINT8_MAX;
                            for (const auto &[locators, type]: connections_to_target_raw) {
                                for (const auto &[depth, locator]: locators) {
                                    if (depth < min_depth) {
                                        min_depth = depth;
                                    }
                                    if (depth == min_depth) {
                                        connections_to_target.push_back({{locator}, type});
                                    }
                                }
                            }
                            _logger->debug("[Simuzilla] [{}] (B.1.1.2) Ending.", display(_uuid));
#else
                            std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target =
                                        get_connections_to(_source_identifier.first);
#endif
                            _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                           ref_message->display());
                            _dispatcher->send_message(ref_message, connections_to_target.front().first, _source_identifier.second);
                            _logger->debug("[Handler] [{}] (B.1.1.2) Create a REF message and send it back.",
                                           display(_uuid));

                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (B.1.1.3) Terminate thread.
                            stop();
                            _logger->debug("[Handler] [{}] (B.1.1.3) Terminate thread.", display(_uuid));
                            return;
                        }
                    } else {
                        _logger->debug("[Handler] [{}] (B.1) Am I the listener? No.", display(_uuid));

                        // (B.1.2.1) Is there a bid in Store for the UUID?
                        if (!is_bid_in_store(ear_message->uuid())) {
                            _logger->debug("[Handler] [{}] (B.1.2.1) Is there a bid in Store for the UUID? No.",
                                           display(_uuid));

                            // (B.1.2.1.2.1) Can R be performed with priority p?
                            auto *position =
                                    _resources->available_for_performance(_reservation, ear_message->priority());
                            if (position == nullptr) {
                                _reservation = position;

                                _logger->debug("[Handler] [{}] (B.1.2.1.2.1) Can R be performed with priority p? No.",
                                               display(_uuid));

                                _logger->warn("[Handler] [{}] The request admission cannot be performed here.",
                                              display(_uuid));

                                // (B.1.1.2) Create a REF message and send it.
                                REF *ref_message = new REF(_uuid);
#ifdef FROM_SIMUZILLA
                                _logger->debug("[Simuzilla] [{}] (B.1.1.2) Starting.", display(_uuid));
                                std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                        get_connections_to(_source_identifier.first.at(0));
                                _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                               display(_uuid), connections_to_target_raw.size(),
                                               connections_to_target_raw.size() == 1 ? "" : "s",
                                               _source_identifier.first.at(0));
                                for (const auto &[connection, type]: connections_to_target_raw) {
                                    _logger->trace("[Handler]                               |-> {} with depth {}",
                                                   connection.at(0).second, connection.at(0).first);
                                }
                                std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target{};
                                uint8_t min_depth = UINT8_MAX;
                                for (const auto &[locators, type]: connections_to_target_raw) {
                                    for (const auto &[depth, locator]: locators) {
                                        if (depth < min_depth) {
                                            min_depth = depth;
                                        }
                                        if (depth == min_depth) {
                                            connections_to_target.push_back({{locator}, type});
                                        }
                                    }
                                }
                                _logger->debug("[Simuzilla] [{}] (B.1.1.2) Ending.", display(_uuid));
#else
                                std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target =
                                        get_connections_to(_source_identifier.first);
#endif
                                _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                               ref_message->display());
                                _dispatcher->send_message(ref_message, connections_to_target.front().first, _source_identifier.second);
                                _logger->debug("[Handler] [{}] (B.1.1.2) Create a REF message and send it back.",
                                               display(_uuid));

                                old_state = _state;
                                _state = HandlerState::CLOSED;
                                _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.",
                                               display(_uuid), handler_state_to_string(old_state),
                                               handler_state_to_string(_state));

                                // (B.1.1.3) Terminate thread.
                                stop();
                                _logger->debug("[Handler] [{}] (B.1.1.3) Terminate thread.", display(_uuid));
                                return;
                            } else {
                                _logger->debug("[Handler] [{}] (B.1.2.1.2.1) Can R be performed with priority p? Yes.",
                                               display(_uuid));

                                // (B.1.2.1.2.2) Pre-reserve R with UUID in Store.
                                _reservation = _resources->mark_pre_reservation(_reservation);
                                _logger->debug("[Handler] [{}] (B.1.2.1.2.2) Pre-reserve R with UUID in Store.",
                                               display(_uuid));

                                // (B.1.2.1.1.1) Get connections towards listener(s).
                                std::vector<uint8_t> target{};
                                switch (listener_field_length) {
                                    case RANK_EAR_MESSAGE_LEN_LT_CODE_0:
                                        target.push_back(listener_field[0]);
                                        break;
                                    case RANK_EAR_MESSAGE_LEN_LT_IP4: {
                                        for (int byte = 0; byte != 4; byte++) {
                                            target.push_back(listener_field[byte]);
                                        }
                                    }
                                        break;
                                    case RANK_EAR_MESSAGE_LEN_LT_MAC: {
                                        for (int byte = 0; byte != 6; byte++) {
                                            target.push_back(listener_field[byte]);
                                        }
                                    }
                                        break;
                                    case RANK_EAR_MESSAGE_LEN_LT_IP6: {
                                        for (int byte = 0; byte != 16; byte++) {
                                            target.push_back(listener_field[byte]);
                                        }
                                    }
                                        break;
                                    default:
                                        throw std::exception();  // TODO
                                }
#ifdef FROM_SIMUZILLA
                                std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                        get_connections_to(target.at(0));
                                _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                               display(_uuid), connections_to_target_raw.size(),
                                               connections_to_target_raw.size() == 1 ? "" : "s", target.at(0));
                                for (const auto &[connection, type]: connections_to_target_raw) {
                                    _logger->trace("               -> {} with depth {}", connection.at(0).second,
                                                   connection.at(0).first);
                                }
                                std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target{};
                                uint8_t min_depth = UINT8_MAX;
                                for (const auto &[locators, type]: connections_to_target_raw) {
                                    for (const auto &[depth, locator]: locators) {
                                        if (depth < min_depth) {
                                            min_depth = depth;
                                        }
                                        if (depth == min_depth) {
                                            connections_to_target.push_back({{locator}, type});
                                        }
                                    }
                                }
#else
                                std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target =
                                        get_connections_to(target);
#endif
                                _logger->debug(
                                        "[Handler] [{}] (B.1.2.1.1.1) Get connections towards listener (N). Found {} connection{}.",
                                        display(_uuid), connections_to_target.size(),
                                        connections_to_target.size() == 1 ? "" : "s");

                                // (B.1.2.2) Depending on the cardinal of connections...
                                switch (connections_cardinal(connections_to_target)) {
                                    case 0: {
                                        _logger->debug("[Handler] [{}] (B.1.2.2) What is the cardinal of N? Zero.",
                                                       display(_uuid));

                                        // If no connection is found...
                                        // (B.1.2.2.3.1) Create REF message and send it.
                                        REF *ref_message = new REF(_uuid);
                                        _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                                       ref_message->display());
#ifdef FROM_SIMUZILLA
                                        _logger->debug("[Simuzilla] [{}] (B.1.2.2.3.1) Starting.", display(_uuid));
                                        _logger->debug("[Simuzilla] [{}] (B.1.2.2.3.1) Ending.", display(_uuid));
#endif
                                        _dispatcher->send_message(ref_message, _source_identifier.first,
                                                                  _source_identifier.second);
                                        _logger->debug(
                                                "[Handler] [{}] (B.1.2.2.3.1) If no connection is found... create REF message and send it back.",
                                                display(_uuid));

                                        old_state = _state;
                                        _state = HandlerState::CLOSED;
                                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.",
                                                       display(_uuid), handler_state_to_string(old_state),
                                                       handler_state_to_string(_state));

                                        // (B.1.2.2.3.2) Terminate thread.
                                        stop();
                                        _logger->debug("[Handler] [{}] (B.1.2.2.3.2) Terminate thread.",
                                                       display(_uuid));
                                        return;
                                    }
                                        break;
                                    case 1: {
                                        _logger->debug("[Handler] [{}] (B.1.2.2) What is the cardinal of N? One.",
                                                       display(_uuid));

                                        // If only one connection is found...
                                        // (B.1.2.2.2.1) Create EAR message and send it.
                                        std::array<uint8_t, 16> listener{};
#ifdef FROM_SIMUZILLA
                                        _logger->debug("[Simuzilla] [{}] (B.1.2.2.2.1) Starting.", display(_uuid));
                                        listener.at(0) = _reservation->listener().at(0);
                                        _logger->debug("[Simuzilla] [{}] (B.1.2.2.2.1) Ending.", display(_uuid));
#else
                                        std::copy_n(ear_message->listener().begin(), ear_message->listener_length(), listener.begin());
#endif
                                        EAR *new_ear_message =
                                                new EAR(_uuid, _reservation->priority(),
                                                        _reservation->listener_length(),
                                                        listener, _reservation->requirements());
                                        _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                                       new_ear_message->display());
                                        _dispatcher->send_message(new_ear_message, connections_to_target.front().first,
                                                                  connections_to_target.front().second);
                                        _logger->debug("[Handler] [{}] (B.1.2.2.2.1) Create EAR message and send it.",
                                                       display(_uuid));

                                        // Change state to PRE_RESERVED.
                                        old_state = _state;
                                        _state = HandlerState::PRE_RESERVED;
                                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.",
                                                       display(_uuid), handler_state_to_string(old_state),
                                                       handler_state_to_string(_state));

                                        // (B.1.2.2.2.2) Add message source address as reservation past node.
                                        _reservation->set_past_node(_source_identifier);
                                        _logger->debug(
                                                "[Handler] [{}] (B.1.2.2.2.2) Add message source address as reservation past node.",
                                                display(_uuid));

                                        // (B.1.2.2.2.3(bis)) Begin timer for EAR timeout.
                                        _timeout_handler->initiate_timeout(this, TimeoutType::EAR);
                                        _logger->debug("[Handler] [{}] (B.1.2.2.2.3(bis)) Begin timer for EAR timeout.",
                                                       display(_uuid));

                                        // (B.1.2.2.2.3) Terminate thread.
                                        stop();
                                        _logger->debug("[Handler] [{}] (B.1.2.2.2.3) Terminate thread.",
                                                       display(_uuid));
                                    }
                                        break;
                                    default: {
                                        _logger->debug(
                                                "[Handler] [{}] (B.1.2.2) What is the cardinal of N? More than one.",
                                                display(_uuid));

                                        // Save the cardinal of connections in the handler's attribute of waiting bids and reset arriving counter.
                                        _waiting_bids = connections_to_target.size();
                                        _arriving_bids = 0;

                                        // If more than one connection is found...
                                        // (B.1.2.2.1.1) For each connection create a MAR message and send it.
                                        for (const auto &intermediate: connections_to_target) {
                                            std::array<uint8_t, 16> listener{};
#ifdef FROM_SIMUZILLA
                                            _logger->debug("[Simuzilla] [{}] (B.1.2.2.1.1) Starting.", display(_uuid));
                                            listener.at(0) = _reservation->listener().at(0);
                                            _logger->debug("[Simuzilla] [{}] (B.1.2.2.1.1) Ending.", display(_uuid));
#else
                                            std::copy_n(ear_message->listener().begin(), ear_message->listener_length(), listener.begin());
#endif
                                            MAR *mar_message =
                                                    new MAR(_uuid, _reservation->priority(),
                                                            _reservation->listener_length(),
                                                            listener,
                                                            _reservation->requirements());
                                            _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                                           mar_message->display());
                                            _dispatcher->send_message(mar_message, intermediate.first,
                                                                      intermediate.second);
                                            _logger->debug("[Handler] [{}] (B.1.2.2.1.1) Create a MAR message and send it.",
                                                           display(_uuid));
                                        }

                                        // Change state to PRE_RESERVED.
                                        old_state = _state;
                                        _state = HandlerState::AUCTION_WAITING;
                                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.",
                                                       display(_uuid), handler_state_to_string(old_state),
                                                       handler_state_to_string(_state));

                                        // (B.1.2.2.1.2) Add message source address as reservation past node.
                                        _reservation->set_past_node(_source_identifier);
                                        _logger->debug(
                                                "[Handler] [{}] (B.1.2.2.1.2) Add message source address as reservation past node.",
                                                display(_uuid));

                                        // (B.1.2.2.1.3(bis)) Begin timer for MAR timeout.
                                        _timeout_handler->initiate_timeout(this, TimeoutType::MAR);
                                        _logger->debug("[Handler] [{}] (B.1.2.2.1.3(bis)) Begin timer for MAR timeout.",
                                                       display(_uuid));

                                        // (B.1.2.2.1.3) Terminate thread.
                                        stop();
                                        _logger->debug("[Handler] [{}] (B.1.2.2.1.3) Terminate thread.",
                                                       display(_uuid));
                                    }
                                        break;
                                }
                            }
                        } else {
                            _logger->debug("[Handler] [{}] (B.1.2.1) Is there a bid in Store for the UUID? Yes.",
                                           display(_uuid));

                            for (auto& reservation : _resources->reservations()) {
                                if (reservation.uuid() == _uuid) {
                                    _reservation = &reservation;
                                }
                            }

                            //* Snip of code copied from above. Look for (*//) to end the copied block.
                            // (B.1.2.1.1.1) Get connections towards listener(s).
                            std::vector<uint8_t> target{};
                            switch (listener_field_length) {
                                case RANK_EAR_MESSAGE_LEN_LT_CODE_0:
                                    target.push_back(listener_field[0]);
                                    break;
                                case RANK_EAR_MESSAGE_LEN_LT_IP4: {
                                    for (int byte = 0; byte != 4; byte++) {
                                        target.push_back(listener_field[byte]);
                                    }
                                }
                                    break;
                                case RANK_EAR_MESSAGE_LEN_LT_MAC: {
                                    for (int byte = 0; byte != 6; byte++) {
                                        target.push_back(listener_field[byte]);
                                    }
                                }
                                    break;
                                case RANK_EAR_MESSAGE_LEN_LT_IP6: {
                                    for (int byte = 0; byte != 16; byte++) {
                                        target.push_back(listener_field[byte]);
                                    }
                                }
                                    break;
                                default:
                                    throw std::exception();  // TODO
                            }
#ifdef FROM_SIMUZILLA
                            std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                    get_connections_to(target.at(0));
                            _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                           display(_uuid), connections_to_target_raw.size(),
                                           connections_to_target_raw.size() == 1 ? "" : "s", target.at(0));
                            for (const auto &[connection, type]: connections_to_target_raw) {
                                _logger->trace("               -> {} with depth {}", connection.at(0).second,
                                               connection.at(0).first);
                            }
                            std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target{};
                            uint8_t min_depth = UINT8_MAX;
                            for (const auto &[locators, type]: connections_to_target_raw) {
                                for (const auto &[depth, locator]: locators) {
                                    if (depth < min_depth) {
                                        min_depth = depth;
                                    }
                                    if (depth == min_depth) {
                                        connections_to_target.push_back({{locator}, type});
                                    }
                                }
                            }
#else
                            std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target =
                                        get_connections_to(target);
#endif
                            _logger->debug(
                                    "[Handler] [{}] (B.1.2.1.1.1) Get connections towards listener (N). Found {} connection{}.",
                                    display(_uuid), connections_to_target.size(),
                                    connections_to_target.size() == 1 ? "" : "s");

                            // (B.1.2.2) Depending on the cardinal of connections...
                            switch (connections_cardinal(connections_to_target)) {
                                case 0: {
                                    _logger->debug("[Handler] [{}] (B.1.2.2) What is the cardinal of N? Zero.",
                                                   display(_uuid));

                                    // If no connection is found...
                                    // (B.1.2.2.3.1) Create REF message and send it.
                                    REF *ref_message = new REF(_uuid);
                                    _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                                   ref_message->display());
#ifdef FROM_SIMUZILLA
                                    _logger->debug("[Simuzilla] [{}] (B.1.2.2.3.1) Starting.", display(_uuid));
                                    _logger->debug("[Simuzilla] [{}] (B.1.2.2.3.1) Ending.", display(_uuid));
#endif
                                    _dispatcher->send_message(ref_message, _source_identifier.first,
                                                              _source_identifier.second);
                                    _logger->debug(
                                            "[Handler] [{}] (B.1.2.2.3.1) If no connection is found... create REF message and send it back.",
                                            display(_uuid));

                                    old_state = _state;
                                    _state = HandlerState::CLOSED;
                                    _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.",
                                                   display(_uuid), handler_state_to_string(old_state),
                                                   handler_state_to_string(_state));

                                    // (B.1.2.2.3.2) Terminate thread.
                                    stop();
                                    _logger->debug("[Handler] [{}] (B.1.2.2.3.2) Terminate thread.",
                                                   display(_uuid));
                                    return;
                                }
                                    break;
                                case 1: {
                                    _logger->debug("[Handler] [{}] (B.1.2.2) What is the cardinal of N? One.",
                                                   display(_uuid));

                                    // If only one connection is found...
                                    // (B.1.2.2.2.1) Create EAR message and send it.
                                    std::array<uint8_t, 16> listener{};
#ifdef FROM_SIMUZILLA
                                    _logger->debug("[Simuzilla] [{}] (B.1.2.2.2.1) Starting.", display(_uuid));
                                    listener.at(0) = _reservation->listener().at(0);
                                    _logger->debug("[Simuzilla] [{}] (B.1.2.2.2.1) Ending.", display(_uuid));
#else
                                    std::copy_n(ear_message->listener().begin(), ear_message->listener_length(), listener.begin());
#endif
                                    EAR *new_ear_message =
                                            new EAR(_uuid, _reservation->priority(),
                                                    _reservation->listener_length(),
                                                    listener, _reservation->requirements());
                                    _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                                   new_ear_message->display());
                                    _dispatcher->send_message(new_ear_message, connections_to_target.front().first,
                                                              connections_to_target.front().second);
                                    _logger->debug("[Handler] [{}] (B.1.2.2.2.1) Create EAR message and send it.",
                                                   display(_uuid));

                                    // Change state to PRE_RESERVED.
                                    old_state = _state;
                                    _state = HandlerState::PRE_RESERVED;
                                    _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.",
                                                   display(_uuid), handler_state_to_string(old_state),
                                                   handler_state_to_string(_state));

                                    // (B.1.2.2.2.2) Add message source address as reservation past node.
                                    _reservation->set_past_node(_source_identifier);
                                    _logger->debug(
                                            "[Handler] [{}] (B.1.2.2.2.2) Add message source address as reservation past node.",
                                            display(_uuid));

                                    // (B.1.2.2.2.3(bis)) Begin timer for EAR timeout.
                                    _timeout_handler->initiate_timeout(this, TimeoutType::EAR);
                                    _logger->debug("[Handler] [{}] (B.1.2.2.2.3(bis)) Begin timer for EAR timeout.",
                                                   display(_uuid));

                                    // (B.1.2.2.2.3) Terminate thread.
                                    stop();
                                    _logger->debug("[Handler] [{}] (B.1.2.2.2.3) Terminate thread.",
                                                   display(_uuid));
                                }
                                    break;
                                default: {
                                    _logger->debug(
                                            "[Handler] [{}] (B.1.2.2) What is the cardinal of N? More than one.",
                                            display(_uuid));

                                    // Save the cardinal of connections in the handler's attribute of waiting bids and reset arriving counter.
                                    _waiting_bids = connections_to_target.size();
                                    _arriving_bids = 0;

                                    // If more than one connection is found...
                                    // (B.1.2.2.1.1) For each connection create a MAR message and send it.
                                    for (const auto &intermediate: connections_to_target) {
                                        std::array<uint8_t, 16> listener{};
#ifdef FROM_SIMUZILLA
                                        _logger->debug("[Simuzilla] [{}] (B.1.2.2.1.1) Starting.", display(_uuid));
                                        listener.at(0) = _reservation->listener().at(0);
                                        _logger->debug("[Simuzilla] [{}] (B.1.2.2.1.1) Ending.", display(_uuid));
#else
                                        std::copy_n(ear_message->listener().begin(), ear_message->listener_length(), listener.begin());
#endif
                                        MAR *mar_message =
                                                new MAR(_uuid, _reservation->priority(),
                                                        _reservation->listener_length(),
                                                        listener,
                                                        _reservation->requirements());
                                        _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                                       mar_message->display());
                                        _dispatcher->send_message(mar_message, intermediate.first,
                                                                  intermediate.second);
                                        _logger->debug("[Handler] [{}] (B.1.2.2.1.1) Create a MAR message and send it.",
                                                       display(_uuid));
                                    }

                                    // Change state to PRE_RESERVED.
                                    old_state = _state;
                                    _state = HandlerState::AUCTION_WAITING;
                                    _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.",
                                                   display(_uuid), handler_state_to_string(old_state),
                                                   handler_state_to_string(_state));

                                    // (B.1.2.2.1.2) Add message source address as reservation past node.
                                    _reservation->set_past_node(_source_identifier);
                                    _logger->debug(
                                            "[Handler] [{}] (B.1.2.2.1.2) Add message source address as reservation past node.",
                                            display(_uuid));

                                    // (B.1.2.2.1.3(bis)) Begin timer for MAR timeout.
                                    _timeout_handler->initiate_timeout(this, TimeoutType::MAR);
                                    _logger->debug("[Handler] [{}] (B.1.2.2.1.3(bis)) Begin timer for MAR timeout.",
                                                   display(_uuid));

                                    // (B.1.2.2.1.3) Terminate thread.
                                    stop();
                                    _logger->debug("[Handler] [{}] (B.1.2.2.1.3) Terminate thread.",
                                                   display(_uuid));
                                }
                                    break;
                            } // *// End of the copied block.
                        }
                    }
                }
                    break;
                case MessageType::MAR: {
                    auto mar_message = dynamic_cast<MAR *>(_message);

                    // Change state to AUCTION_BIDDING.
                    old_state = _state;
                    _state = HandlerState::AUCTION_BIDDING;
                    _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                   handler_state_to_string(old_state),
                                   handler_state_to_string(_state));

                    auto listener_field = mar_message->listener();
                    auto listener_field_length = mar_message->listener_length();
                    //bool i_am_listener = false;
                    switch (listener_field_length) {
#ifdef FROM_SIMUZILLA
                        case RANK_MAR_MESSAGE_LEN_LT_CODE_0: {
                            uint8_t simuzilla_address = listener_field.at(0);
                            _logger->trace("[Handler] [{}] Testing if I am id {}.", display(_uuid), simuzilla_address);
                            //i_am_listener = simulated_is_me(simuzilla_address);

                            // Update inner reservation with the requirements of the message.
                            try {
                                produce_reservation(mar_message->requirements(), mar_message->priority(),
                                                    std::vector<uint8_t>({simuzilla_address}));
                            } catch (const std::invalid_argument &ia) {
                                _logger->error(
                                        "[Handler] [{}] Received message has non-compliant list of requirements. Ignoring messages and deleting handler.",
                                        display(_uuid));
                                old_state = _state;
                                _state = HandlerState::CLOSED;
                                stop();
                                return;
                            }
                        }
                            break;
#endif
                        case RANK_MAR_MESSAGE_LEN_LT_IP4: {
                            std::array<uint8_t, 4> ip4_address{};
                            for (int byte = 0; byte != 4; byte++) {
                                ip4_address[byte] = listener_field[byte];
                            }
                            //i_am_listener = is_me(ip4_address);

                            // Update inner reservation with the requirements of the message.
                            produce_reservation(mar_message->requirements(), mar_message->priority(),
                                                std::vector<uint8_t>(ip4_address.begin(), ip4_address.end()));
                        }
                            break;
                        case RANK_MAR_MESSAGE_LEN_LT_MAC: {
                            std::array<uint8_t, 6> mac_address{};
                            for (int byte = 0; byte != 6; byte++) {
                                mac_address[byte] = listener_field[byte];
                            }
                            //i_am_listener = is_me(mac_address);

                            // Update inner reservation with the requirements of the message.
                            produce_reservation(mar_message->requirements(), mar_message->priority(),
                                                std::vector<uint8_t>(mac_address.begin(), mac_address.end()));
                        }
                            break;
                        case RANK_MAR_MESSAGE_LEN_LT_IP6: {
                            std::array<uint8_t, 16> ip6_address{};
                            for (int byte = 0; byte != 16; byte++) {
                                ip6_address[byte] = listener_field[byte];
                            }
                            //i_am_listener = is_me(ip6_address);

                            // Update inner reservation with the requirements of the message.
                            produce_reservation(mar_message->requirements(), mar_message->priority(),
                                                std::vector<uint8_t>(ip6_address.begin(), ip6_address.end()));
                        }
                            break;
                        default:
                            throw std::exception();  // TODO
                    }

                    // Mark reservation UUID.
                    _reservation->set_uuid(_uuid);

                    _logger->debug("[Process] [{}] (A.4) What is this message type? MAR.", display(_uuid));

                    // (C.1) Can R be performed with priority p?
                    auto *position = _resources->available_for_performance(_reservation, mar_message->priority());
                    if (position != nullptr) {
                        _reservation = position;

                        _logger->debug("[Handler] [{}] (C.1) Can R be performed with priority p? Yes.", display(_uuid));

                        // (C.1.2.1) Save bid as b.
                        float bid_value = position->last_bid(); //  _resources->estimate_bid(*_reservation); // TODO
                        _logger->debug("[Handler] [{}] (C.1.2.1) Save bid as b.", display(_uuid));
                        _logger->trace("[Handler] [{}] The bid was estimated of {}.", display(_uuid), bid_value);

                        // (C.1.2.2) Create BID message and send it.
                        BID *bid_message = new BID(_uuid, bid_value);
                        std::vector<uint8_t> target;
#ifdef FROM_SIMUZILLA
                        _logger->debug("[Simuzilla] [{}] (C.1.2.2) Starting.", display(_uuid));
                        std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                get_connections_to(_source_identifier.first.at(0));
                        _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                       display(_uuid), connections_to_target_raw.size(),
                                       connections_to_target_raw.size() == 1 ? "" : "s",
                                       _source_identifier.first.at(0));
                        for (const auto &[connection, type]: connections_to_target_raw) {
                            _logger->trace("[Handler]                               -> {} with depth {}",
                                           connection.at(0).second, connection.at(0).first);
                        }
                        target = {connections_to_target_raw.at(0).first.at(0).second};
                        _logger->debug("[Simuzilla] [{}] (C.1.2.2) Ending.", display(_uuid));
#else
                        target = _source_identifier.first;
#endif
                        _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid), bid_message->display());
                        _dispatcher->send_message(bid_message, target, _source_identifier.second);
                        _logger->debug("[Handler] [{}] (C.1.2.2) Create BID message and send it.", display(_uuid));

                        // Change state to PRE_RESERVED.
                        old_state = _state;
                        _state = HandlerState::PRE_RESERVED;
                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                       handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (C.1.2.3) Pre-reserve R with UUID in the store.
                        _reservation->update_last_bid(bid_value);
                        _reservation = _resources->mark_pre_reservation(_reservation);
                        _logger->debug("[Handler] [{}] (C.1.2.3) Pre-reserve R with UUID in the store.",
                                       display(_uuid));

                        // (C.1.2.4(bis)) Begin timer for BID timeout.
                        _timeout_handler->initiate_timeout(this, TimeoutType::BID);
                        _logger->debug("[Handler] [{}] (C.1.2.4(bis)) Begin timer for BID timeout.", display(_uuid));

                        // (C.1.2.4) Terminate thread.
                        stop();
                        _logger->debug("[Handler] [{}] (C.1.2.4) Terminate thread.", display(_uuid));
                    } else {
                        _logger->debug("[Handler] [{}] (C.1) Can R be performed with priority p? No.", display(_uuid));

                        // (C.1.1.1) Create zeroed-bid message and send it.
                        BID *bid_message = new BID(_uuid, 0);
                        std::vector<uint8_t> target;
#ifdef FROM_SIMUZILLA
                        _logger->debug("[Simuzilla] [{}] (C.1.1.1) Starting.", display(_uuid));
                        std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                get_connections_to(_source_identifier.first.at(0));
                        _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                       display(_uuid), connections_to_target_raw.size(),
                                       connections_to_target_raw.size() == 1 ? "" : "s",
                                       _source_identifier.first.at(0));
                        for (const auto &[connection, type]: connections_to_target_raw) {
                            _logger->trace("[Handler]                               -> {} with depth {}",
                                           connection.at(0).second, connection.at(0).first);
                        }
                        target = {connections_to_target_raw.at(0).first.at(0).second};
                        _logger->debug("[Simuzilla] [{}] (C.1.1.1) Ending.", display(_uuid));
#else
                        target = _source_identifier.first;
#endif
                        _dispatcher->send_message(bid_message, _source_identifier.first, _source_identifier.second);
                        _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid), bid_message->display());
                        _logger->debug("[Handler] [{}] (C.1.1.1) Create zeroed-bid message and send it.",
                                       display(_uuid));

                        // Change handler's state to CLOSED.
                        old_state = _state;
                        _state = HandlerState::CLOSED;
                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                       handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (C.1.1.2) Terminate thread.
                        stop();
                        _logger->debug("[Handler] [{}] (C.1.1.2) Terminate thread.", display(_uuid));
                        break;
                    }
                }
                    break;
                case MessageType::BID: {
                    _logger->debug("[Process] [{}] (A.4) What is this message type? BID.", display(_uuid));

                    // (D.1) Is auction waiting?
                    if (_state != HandlerState::AUCTION_WAITING) {
                        _logger->debug("[Handler] [{}] (D.1) Is auction waiting? No.", display(_uuid));
                        _logger->warn("[Handler] [{}] Handler received an unexpected BID message.", display(_uuid));

                        old_state = _state;
                        _state = HandlerState::CLOSED;
                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                       handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (D.1.2.1) Terminate thread.
                        stop();
                        _logger->debug("[Handler] [{}] (D.1.2.1) Terminate thread.");
                        return;
                    }
                    _logger->debug("[Handler] [{}] (D.1) Is auction waiting? Yes.", display(_uuid));

                    auto bid_message = dynamic_cast<BID *>(_message);

                    // Add bid in the BidSet.
                    {
                        std::lock_guard<std::mutex> guard(_bids_locker);
                        _bids.insert({bid_message->value(), _source_identifier});
                        _arriving_bids += 1;
                    }

                    // (D.1.1.2) Wait for all bids.
                    _timeout_handler->initiate_timeout(this, TimeoutType::AUCT);
                    _logger->debug("[Handler] [{}] (D.1.1.2) Wait for all bids.", display(_uuid));
                    std::unique_lock auction_locker(_auction_mutex);
                    _auction_wait.wait(auction_locker);

                    // (D.1.1.3) Check the cardinal of bids.
                    if (cardinal_bids() == 0) {
                        _logger->debug("[Handler] [{}] (D.1.1.3) Check the cardinal of bids. Zero", display(_uuid));

                        // (D.1.1.3.2.1) Create REF message and send it.
                        REF *ref_message = new REF(_uuid);
#ifdef FROM_SIMUZILLA
                        _logger->debug("[Simuzilla] [{}] (D.1.1.3.2.1) Starting.", display(_uuid));
                        _logger->debug("[Simuzilla] [{}] (D.1.1.3.2.1) Ending.", display(_uuid));
#endif
                        _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid), ref_message->display());
                        _dispatcher->send_message(ref_message, _source_identifier.first, _source_identifier.second);
                        _logger->debug("[Handler] [{}] (D.1.1.3.2.1) Create REF message and send it.", display(_uuid));

                        // Change the state to CLOSED.
                        old_state = _state;
                        _state = HandlerState::CLOSED;
                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                       handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (D.1.1.3.2.2) Terminate thread.
                        stop();
                        _logger->debug("[Handler] [{}] (D.1.1.3.2.2) Terminate thread.", display(_uuid));
                        break;
                    } else {
                        _logger->debug("[Handler] [{}] (D.1.1.3) Check the cardinal of bids. More than zero.",
                                       display(_uuid));

                        // (D.1.1.3.1.1) Is the maximum bid unique?
                        auto maximum_bid = max_bids();
                        if (is_max_bid_unique(maximum_bid)) {
                            _logger->debug("[Handler] [{}] (D.1.1.3.1.1) Is the maximum bid unique? Yes.",
                                           display(_uuid));

                            // (D.1.1.3.1.1.1.1) As it is unique, send an EAR message to it.
                            std::array<uint8_t, 16> listener{};
#ifdef FROM_SIMUZILLA
                            listener.at(0) = _reservation->listener().at(0);
#else
                            std::copy_n(_reservation->listener().begin(), _reservation->listener_length(), listener.begin());
#endif
                            EAR *ear_message = new EAR(_uuid, _reservation->priority(), _reservation->listener_length(),
                                                       listener,
                                                       make_payload_length(_reservation->requirements()),
                                                       make_payload(_reservation->requirements()));
                            _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                           ear_message->display());
                            std::vector<uint8_t> target;
#ifdef FROM_SIMUZILLA
                            _logger->debug("[Simuzilla] [{}] (D.1.1.3.1.1.1.1) Starting.", display(_uuid));
                            std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                    get_connections_to(maximum_bid.begin()->first.at(0));
                            _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                           display(_uuid), connections_to_target_raw.size(),
                                           connections_to_target_raw.size() == 1 ? "" : "s",
                                           _source_identifier.first.at(0));
                            for (const auto &[connection, type]: connections_to_target_raw) {
                                _logger->trace("[Handler]                               -> {} with depth {}",
                                               connection.at(0).second, connection.at(0).first);
                            }
                            target = {connections_to_target_raw.at(0).first.at(0).second};
                            _logger->debug("[Simuzilla] [{}] (D.1.1.3.1.1.1.1) Ending.", display(_uuid));
#else
                            target = minimum_bids.begin()->first;
#endif
                            _dispatcher->send_message(ear_message, target, maximum_bid.begin()->second);
                            _logger->debug(
                                    "[Handler] [{}] (D.1.1.3.1.1.1.1) As it is unique, send an EAR message to it.",
                                    display(_uuid));

                            // Change the state to PRE_RESERVED.
                            old_state = _state;
                            _state = HandlerState::PRE_RESERVED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // // (D.1.1.3.1.1.1.2) Add message source address as reservation past node.
                            // _logger->debug("[Handler] [{}] (D.1.1.3.1.1.1.2) Add message source address as reservation past node.", display(_uuid));
                            // //_reservation->set_past_node(_source_identifier);

                            // (D.1.1.3.1.1.1.2(bis)) Begin timer for EAR timeout.
                            _timeout_handler->initiate_timeout(this, TimeoutType::EAR);
                            _logger->debug("[Handler] [{}] (D.1.1.3.1.1.1.2(bis)) Begin timer for EAR timeout.",
                                           display(_uuid));

                            // (D.1.1.3.1.1.1.2) Terminate thread.
                            stop();
                            _logger->debug("[Handler] [{}] (D.1.1.3.1.1.1.2) Terminate thread.", display(_uuid));
                            break;
                        } else {
                            _logger->debug("[Handler] [{}] (D.1.1.3.1.1) Is the maximum bid unique? No.",
                                           display(_uuid));

                            // (D.1.1.3.1.1.2.1) Create a new UUID for each max(B) node and (D.1.1.3.1.1.2.2) Save
                            // translations UUID to UUID* in TranslationTable.
                            _logger->debug("[Handler] [{}] (D.1.1.3.1.1.2.1) Create new UUID* for each max(B) node.",
                                           display(_uuid));
                            for (const auto &bid_target: maximum_bid) {
                                auto new_uuid = create_translation(_uuid);
                                _logger->trace("[Handler] [{}] (D.1.1.3.1.1.2.1) Created {} as a new UUID*.",
                                               display(_uuid), display(new_uuid));

                                _logger->debug(
                                        "[Handler] [{}] (D.1.1.3.1.1.2.2) Save [UUID, UUID*] in Translation Table",
                                        display(_uuid));

                                // (D.1.1.3.1.1.2.3) Create EAR message and send it to each min(B) node, with UUID*.
                                std::array<uint8_t, 16> listener{};
#ifdef FROM_SIMUZILLA
                                listener.at(0) = _reservation->listener().at(0);
#else
                                std::copy_n(ear_message->listener().begin(), ear_message->listener_length(), listener.begin());
#endif
                                EAR *ear_message = new EAR(new_uuid, _reservation->priority(),
                                                           _reservation->listener_length(), listener,
                                                           make_payload_length(_reservation->requirements()),
                                                           make_payload(_reservation->requirements()));
                                _logger->trace("[Handler] [{}] Sending message: {}.", display(_uuid),
                                               ear_message->display());
                                std::vector<uint8_t> target;
#ifdef FROM_SIMUZILLA
                                _logger->debug("[Simuzilla] [{}] (D.1.1.3.1.1.2.3) Starting.", display(_uuid));
                                std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                        get_connections_to(bid_target.first.at(0));
                                _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                               display(_uuid), connections_to_target_raw.size(),
                                               connections_to_target_raw.size() == 1 ? "" : "s",
                                               _source_identifier.first.at(0));
                                for (const auto &[connection, type]: connections_to_target_raw) {
                                    _logger->trace("[Handler]                               -> {} with depth {}",
                                                   connection.at(0).second, connection.at(0).first);
                                }
                                target = {connections_to_target_raw.at(0).first.at(0).second};
                                _logger->debug("[Simuzilla] [{}] (D.1.1.3.1.1.2.3) Ending.", display(_uuid));
#else
                                target = minimum_bids.begin()->first;
#endif
                                _dispatcher->send_message(ear_message, target,
                                                          bid_target.second);
                                _logger->debug(
                                        "[Handler] [{}] (D.1.1.3.1.1.2.3) Create EAR message and send it to each min(B) node, with UUID*.",
                                        display(_uuid));

                                // Change the state to PRE_RESERVED.
                                old_state = _state;
                                _state = HandlerState::PRE_RESERVED;
                                _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.",
                                               display(_uuid), handler_state_to_string(old_state),
                                               handler_state_to_string(_state));

                                // // (D.1.1.3.1.1.2.4) Add message source address as reservation past node.
                                // _logger->debug("[Handler] [{}] (D.1.1.3.1.1.2.4) Add message source address as reservation past node.", display(_uuid));
                                // _reservation->set_past_node(_source_identifier);

                                // (D.1.1.3.1.1.2.4(bis)) Begin timer for EAR timeout.
                                _timeout_handler->initiate_timeout(this, TimeoutType::EAR);
                                _logger->debug("[Handler] [{}] (D.1.1.3.1.1.2.4(bis)) Begin timer for EAR timeout.",
                                               display(_uuid));
                                // TODO _timeout_handler->initiate_timeout(this, RANK_EAR_TO_EAR_TIMEOUT);

                                // (D.1.1.3.1.1.2.4) Terminate thread.
                                _logger->debug("[Handler] [{}] (D.1.1.3.1.1.2.4) Terminate thread.", display(_uuid));
                            }
                            stop();
                            break;
                        }
                    }

                    // Clear all the bids from this handler.
                    _logger->trace("[Handler] [{}] Clearing all the bids from this handler.", display(_uuid));
                    clear_bids();
                }
                    break;
                case MessageType::ACC: {
                    auto acc_message = dynamic_cast<ACC *>(_message);

                    if (_state == HandlerState::RESERVED) {
                        _logger->warn("[Handler] [{}] An ACC message has beed received for an already reserved request. Ignoring...", display(_uuid));
                        stop();
                        break;
                    }

                    _logger->debug("[Process] [{}] (A.4) What is this message type? ACC.", display(_uuid));

                    // (E.1) Set pre-reservation as reserved on UUID in Store.
                    _resources->mark_reservation(_reservation);
                    _logger->debug("[Handler] [{}] (E.1) Set pre-reservation as reserved on UUID in Store.",
                                   display(_uuid));

                    // (E.2) Is UUID in the TranslationTable?
                    if (not is_translation_table_empty_for(_uuid)) {
                        _logger->debug("[Handler] [{}] (E.2) Is UUID in the TranslationTable? Yes.", display(_uuid));

                        UUIDv4 original_uuid = locate_original_of(_uuid);

                        {
                            std::lock_guard<std::mutex> lock(*_store_locker);
                            auto stored_item = _store->extract(_uuid);
                            stored_item.key() = original_uuid;
                            _store->insert(std::move(stored_item));
                            _uuid = original_uuid;
                        }

                        // (E.2.1.1) Create ACC message to the original UUID.
                        ACC *new_acc_message = new ACC(original_uuid);
                        _logger->trace("[Handler] [{}] Sending message {}.", display(_uuid),
                                       new_acc_message->display());
                        std::pair<std::vector<uint8_t>, IdentifierType> new_target;
#ifndef FROM_SIMUZILLA
                        new_target = _reservation->past_node();
#else
                        // Get connecting port to targeted entity.
                        _logger->debug("[Simuzilla] [{}] (E.2.1.1) Starting.", display(_uuid));
                        if (_reservation->past_node().second == IdentifierType::Simulation) {
                            auto topology = _dispatcher->get_topology();
                            auto found = std::find(topology.begin(), topology.end(),
                                                   _reservation->past_node().first.at(0));
                            if (found == topology.end()) {
                                _logger->error(
                                        "[Handler] [{}] No direct connection was found relating the targeted node {} to a node port in Simuzilla.",
                                        display(_uuid), _reservation->past_node().first.at(0));
                                stop();
                                break;
                            }
                            new_target.first = {static_cast<uint8_t>(std::distance(topology.begin(), found))};
                            new_target.second = IdentifierType::Simulation;
                        }
                        _logger->debug("[Simuzilla] [{}] (E.2.1.1) Ending.", display(_uuid));
#endif
                        _dispatcher->send_message(new_acc_message, new_target.first, new_target.second); //_source_identifier.first, _source_identifier.second);
                        _logger->debug("[Handler] [{}] (E.2.1.1) Create ACC message to the original UUID.",
                                       display(_uuid));

                        // Change state to RESERVED.
                        old_state = _state;
                        _state = HandlerState::RESERVED;
                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                       handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (E.2.1.2) Add accepting node as next node of the reservation.
                        _reservation->add_next_node(_source_identifier);
                        _logger->debug("[Handler] [{}] (E.2.1.2) Add accepting node as next node of the reservation.",
                                       display(_uuid));

                        // (E.2.1.3) Terminate the thread.
                        stop();
                        _logger->debug("[Handler] [{}] (E.2.1.3) Terminate the thread.", display(_uuid));
                        break;
                    } else {
                        _logger->debug("[Handler] [{}] (E.2) Is UUID in the TranslationTable? No.", display(_uuid));

                        // (E.2.2.1) Is UUID in Origin Set?
                        if (_am_i_origin_for(_uuid)) {
                            _logger->debug("[Handler] [{}] (E.2.2.1) Is UUID in Origin Set? Yes.", display(_uuid));

                            // (E.2.2.1.1.1) Return reservation result to API.
                            _dispatcher->api()->communicate_result(ApiResult::OK, "", _uuid); // TODO
                            _logger->debug("[Handler] [{}] (E.2.2.1.1.1) Return reservation result to the API",
                                           display(_uuid));

                            // (E.2.2.1.1.2) Add accepting node as next node of the reservation.
                            _reservation->add_next_node(_source_identifier);
                            _logger->debug(
                                    "[Handler] [{}] (E.2.2.1.1.2) Add accepting node as next node of the reservation.",
                                    display(_uuid));

                            // Change state to RESERVED.
                            old_state = _state;
                            _state = HandlerState::RESERVED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (E.2.2.1.1.3) Terminate the thread.
                            stop();
                            _logger->debug("[Handler] [{}] (E.2.2.1.1.3) Terminate the thread.", display(_uuid));
                            break;
                        } else {
                            _logger->debug("[Handler] [{}] (E.2.2.1) Is UUID in Origin Set? No.", display(_uuid));

                            std::pair<std::vector<uint8_t>, IdentifierType> new_target;
#ifndef FROM_SIMUZILLA
                            new_target = _reservation->past_node();
#else
                            _logger->debug("[Simuzilla] [{}] (E.2.2.1.2.1) Starting.", display(_uuid));
                            // Get connecting port to targeted entity.
                            if (_reservation->past_node().second == IdentifierType::Simulation) {
                                auto topology = _dispatcher->get_topology();
                                auto found = std::find(topology.begin(), topology.end(),
                                                       _reservation->past_node().first.at(0));
                                if (found == topology.end()) {
                                    _logger->error(
                                            "[Handler] [{}] No direct connection was found relating the targeted node {} to a node port in Simuzilla.",
                                            display(_uuid), _reservation->past_node().first.at(0));
                                    stop();
                                    break;
                                }
                                new_target.first = {static_cast<uint8_t>(std::distance(topology.begin(), found))};
                                new_target.second = IdentifierType::Simulation;
                            }
                            _logger->debug("[Simuzilla] [{}] (E.2.2.1.2.1) Ending.", display(_uuid));
#endif

                            // (E.2.2.1.2.1) Create ACC message to the UUID.
                            ACC *new_acc_message = new ACC(_uuid);
                            _logger->trace("[Handler] [{}] Sending message {}.", display(_uuid),
                                           new_acc_message->display());
                            _dispatcher->send_message(new_acc_message, new_target.first, new_target.second); //_source_identifier.first, _source_identifier.second);
                            _logger->debug("[Handler] [{}] (E.2.2.1.2.1) Create ACC message to the UUID.",
                                           display(_uuid));

                            // Change state to RESERVED.
                            old_state = _state;
                            _state = HandlerState::RESERVED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (E.2.2.1.2.2) Add accepting node as next node of the reservation.
                            _reservation->add_next_node(_source_identifier);
                            _logger->debug(
                                    "[Handler] [{}] (E.2.2.1.2.2) Add accepting node as next node of the reservation.",
                                    display(_uuid));

                            // (E.2.2.1.2.3) Terminate the thread.
                            stop();
                            _logger->debug("[Handler] [{}] (E.2.2.1.2.3) Terminate the thread.", display(_uuid));
                            break;
                        }
                    }
                }
                    break;
                case MessageType::REF: {
                    auto ref_message = dynamic_cast<REF *>(_message);

                    _logger->debug("[Process] [{}] (A.4) What is this message type? REF.", display(_uuid));

                    // (F.1) Delete pre-reservations with UUID in the Store.
                    {
                        std::lock_guard<std::mutex> guard(*_store_locker);

                        for (const auto &[uuid, handler]: *_store) {
                            if (uuid == _uuid) {
                                if (handler->associated_reservation() != nullptr and handler->associated_reservation()->state() == ReservationState::PRE_RESERVED) {
                                    _store->erase(uuid);
                                }
                            }
                        }
                    }
                    _logger->debug("[Handler] [{}] (F.1) Delete pre-reservations with UUID in the Store.",
                                   display(_uuid));

                    // (F.2) Is UUID in the TranslationTable?
                    if (not is_translation_table_empty_for(_uuid)) {
                        _logger->debug("[Handler] [{}] (F.2) Is UUID in the TranslationTable? Yes.", display(_uuid));

                        // (F.2.1.1) Delete UUID* from the Translation Table.
                        {
                            std::lock_guard<std::mutex> guard(*_translation_table_locker);

                            auto old_uuid = _translation_table->at(_uuid);
                            _translation_table->erase(_uuid);
                            _uuid = old_uuid;
                        }
                        _logger->debug("[Handler] [{}] (F.2.1.1) Delete UUID* from the Translation Table.",
                                       display(_uuid));

                        // (F.2.1.2) Is TranslationTable empty?
                        if (not is_translation_table_empty_for(_uuid)) {
                            _logger->debug("[Handler] [{}] (F.2.1.2) Is TranslationTable empty? No.", display(_uuid));

                            // (F.2.2.1.2.1) Create REF message and send it.
                            std::pair<std::vector<uint8_t>, IdentifierType> new_target;
#ifndef FROM_SIMUZILLA
                            new_target = _source_identifier;
#else
                            _logger->debug("[Simuzilla] [{}] (F.2.2.1.2.1) Starting.", display(_uuid));
                            // Get connecting port to targeted entity.
                            if (_source_identifier.second == IdentifierType::Simulation) {
                                auto topology = _dispatcher->get_topology();
                                auto found = std::find(topology.begin(), topology.end(),
                                                       _source_identifier.first.at(0));
                                if (found == topology.end()) {
                                    _logger->error(
                                            "[Handler] [{}] No direct connection was found relating the targeted node {} to a node port in Simuzilla",
                                            display(_uuid), _source_identifier.first.at(0));
                                    stop();
                                    break;
                                }
                                new_target.first = {static_cast<uint8_t>(std::distance(topology.begin(), found))};
                                new_target.second = IdentifierType::Simulation;
                            };
                            _logger->debug("[Simuzilla] [{}] (F.2.2.1.2.1) Ending.", display(_uuid));
#endif

                            REF *new_ref_message = new REF(_uuid);
                            _logger->trace("[Handler] [{}] Sending message {}.", display(_uuid),
                                           new_ref_message->display());
                            _dispatcher->send_message(new_ref_message, new_target.first, new_target.second);
                            _logger->debug("[Handler] [{}] (F.2.2.1.2.1) Create REF message and send it.",
                                           display(_uuid));

                            // Change state to CLOSED.
                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (F.2.2.1.2.2) Terminate thread.
                            stop();
                            _logger->debug("[Handler] [{}] (F.2.2.1.2.2) Terminate thread.", display(_uuid));
                            break;
                        } else {
                            _logger->debug("[Handler] [{}] (F.2.1.2) Is TranslationTable empty? Yes.", display(_uuid));

                            // Change state to CLOSED.
                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));


                            // (F.2.1.3) Terminate the thread.
                            stop();
                            _logger->debug("[Handler] [{}] (F.2.1.3) Terminate the thread.", display(_uuid));
                            break;
                        }
                    } else {
                        _logger->debug("[Handler] [{}] (F.2) Is UUID in the TranslationTable? No.", display(_uuid));

                        // (F.2.2.1) Is UUID in Origin Set?
                        if (_am_i_origin_for(_uuid)) {
                            _logger->debug("[Handler] [{}] (F.2.2.1) Is UUID in Origin Set? Yes.", display(_uuid));

                            // (F.2.2.1.1.1) Return reservation result to API.
                            _dispatcher->api()->communicate_result(ApiResult::FAIL, "", _uuid); // TODO API
                            _logger->debug("[Handler] [{}] (F.2.2.1.1.1) Return reservation result to API.",
                                           display(_uuid));

                            // (F.2.2.1.1.2) Remove UUID from Origin Set.
                            _remove_as_origin(_uuid);
                            _logger->debug("[Handler] [{}] (F.2.2.1.1.2) Remove UUID from Origin Set.", display(_uuid));

                            // Change state to CLOSED.
                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (F.2.2.1.1.3) Terminate thread.
                            stop();
                            _logger->debug("[Handler] [{}] (F.2.2.1.1.3) Terminate thread.", display(_uuid));
                            break;
                        } else {
                            _logger->debug("[Handler] [{}] (F.2.2.1) Is UUID in Origin Set? No.", display(_uuid));

                            std::pair<std::vector<uint8_t>, IdentifierType> new_target;
#ifndef FROM_SIMUZILLA
                            new_target = _source_identifier;
#else
                            // Get connecting port to targeted entity.
                            if (_source_identifier.second == IdentifierType::Simulation) {
                                auto topology = _dispatcher->get_topology();
                                auto found = std::find(topology.begin(), topology.end(),
                                                       _source_identifier.first.at(0));
                                if (found == topology.end()) {
                                    _logger->error(
                                            "[Handler] [{}] No direct connection was found relating the targeted node {} to a node port in Simuzilla",
                                            display(_uuid), _source_identifier.first.at(0));
                                    stop();
                                    break;
                                }
                                new_target.first = {static_cast<uint8_t>(std::distance(topology.begin(), found))};
                                new_target.second = IdentifierType::Simulation;
                            };
#endif

                            // (F.2.2.1.2.1) Create REF message and send it.
                            REF *new_ref_message = new REF(_uuid);
                            _logger->trace("[Handler] [{}] Sending message {}.", display(_uuid),
                                           new_ref_message->display());
                            _dispatcher->send_message(new_ref_message, new_target.first, new_target.second);
                            _logger->debug("[Handler] [{}] (F.2.2.1.2.1) Create REF message and send it.",
                                           display(_uuid));

                            // Change state to CLOSED.
                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (F.2.2.1.2.2) Terminate thread.
                            stop();
                            _logger->debug("[Handler] [{}] (F.2.2.1.2.2) Terminate thread.", display(_uuid));
                            break;
                        }
                    }
                }
                    break;
                case MessageType::REP: {
                    auto rep_message = dynamic_cast<REP *>(_message);

                    // Change state to REPLENISHING.
                    old_state = _state;
                    _state = HandlerState::REPLENISHING;
                    _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                   handler_state_to_string(old_state),
                                   handler_state_to_string(_state));

                    // (G.1) Check if the current node is the message's listener.
                    auto listener_field = rep_message->listener();
                    auto listener_field_length = rep_message->listener_length();
                    bool i_am_listener = false;

                    _logger->debug("[Process] [{}] (A.4) What is this message type? REP.", display(_uuid));
                    switch (listener_field_length) {
#ifdef FROM_SIMUZILLA
                        case RANK_MAR_MESSAGE_LEN_LT_CODE_0: {
                            uint8_t simuzilla_address = listener_field.at(0);
                            _logger->trace("[Handler] [{}] Testing if I am id {}.", display(_uuid), simuzilla_address);
                            i_am_listener = simulated_is_me(simuzilla_address);
                        }
                            break;
#endif
                        case RANK_EAR_MESSAGE_LEN_LT_IP4: {
                            std::array<uint8_t, 4> ip4_address{};
                            for (int byte = 0; byte != 4; byte++) {
                                ip4_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(ip4_address);
                        }
                            break;
                        case RANK_EAR_MESSAGE_LEN_LT_MAC: {
                            std::array<uint8_t, 6> mac_address{};
                            for (int byte = 0; byte != 6; byte++) {
                                mac_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(mac_address);
                        }
                            break;
                        case RANK_EAR_MESSAGE_LEN_LT_IP6: {
                            std::array<uint8_t, 16> ip6_address{};
                            for (int byte = 0; byte != 16; byte++) {
                                ip6_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(ip6_address);
                        }
                            break;
                        default:
                            throw std::exception();  // TODO
                    }
                    if (i_am_listener) {
                        _logger->debug("[Handler] [{}] (G.1) Check if the current node is the message's listener? Yes.",
                                       display(_uuid));

                        // (G.1.1.1) Create REF message and send it.
                        REF *ref_message = new REF(_uuid);
                        std::vector<uint8_t> target;
#ifdef FROM_SIMUZILLA
                        _logger->debug("[Simuzilla] [{}] (G.1.1.1) Starting.", display(_uuid));
                        std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                get_connections_to(_source_identifier.first.at(0));
                        _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                       display(_uuid), connections_to_target_raw.size(),
                                       connections_to_target_raw.size() == 1 ? "" : "s",
                                       _source_identifier.first.at(0));
                        for (const auto &[connection, type]: connections_to_target_raw) {
                            _logger->trace("[Handler]                               -> {} with depth {}",
                                           connection.at(0).second, connection.at(0).first);
                        }
                        target = {connections_to_target_raw.at(0).first.at(0).second};
                        _logger->debug("[Simuzilla] [{}] (G.1.1.1) Ending.", display(_uuid));
#else
                        target = _source_identifier.first;
#endif
                        _logger->trace("[Handler] [{}] Sending message {}.", display(_uuid), ref_message->display());
                        _dispatcher->send_message(ref_message, target, _source_identifier.second);
                        _logger->debug("[Handler] [{}] (G.1.1.1) Create REF message and send it.", display(_uuid));

                        // Change state to CLOSED.
                        old_state = _state;
                        _state = HandlerState::CLOSED;
                        _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                       handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (G.1.1.2) Delete UUID from the Store.
                        stop();
                        _logger->debug("[Handler] [{}] (G.1.1.2) Delete UUID from the Store.", display(_uuid));
                        break;
                    } else {
                        _logger->debug("[Handler] [{}] (G.1) Check if the current node is the message's listener. No.",
                                       display(_uuid));

                        // (G.1.2.1) Is UUID in Store?
                        if (is_uuid_in_store(_uuid)) {
                            _logger->debug("[Handler] [{}] (G.1.2.1) Is UUID in Store? Yes.", display(_uuid));

                            // (G.1.2.1.1.1) Create REP message towards L and send it.
                            REP *new_rep_message = new REP(_uuid, _reservation->listener_length(),
                                                           _reservation->listener());
                            for (const auto &direction: _reservation->next_nodes()) {
                                std::vector<uint8_t> target;
#ifdef FROM_SIMUZILLA
                                _logger->debug("[Simuzilla] [{}] (G.1.2.1.1.1) Starting.", display(_uuid));
                                std::vector<std::pair<std::vector<std::pair<uint8_t, uint8_t>>, IdentifierType>> connections_to_target_raw =
                                        get_connections_to(direction.first.at(0));
                                _logger->trace("[Handler] [{}] Collected {} connection{} to target {}. Possibilities:",
                                               display(_uuid), connections_to_target_raw.size(),
                                               connections_to_target_raw.size() == 1 ? "" : "s",
                                               _source_identifier.first.at(0));
                                for (const auto &[connection, type]: connections_to_target_raw) {
                                    _logger->trace("[Handler]                               -> {} with depth {}",
                                                   connection.at(0).second, connection.at(0).first);
                                }
                                target = {connections_to_target_raw.at(0).first.at(0).second};
                                _logger->debug("[Simuzilla] [{}] (G.1.2.1.1.1) Ending.", display(_uuid));
#else
                                target = _source_identifier.first;
#endif
                                _logger->trace("[Handler] [{}] Sending message {}.", display(_uuid),
                                               new_rep_message->display());
                                _dispatcher->send_message(new_rep_message, target, direction.second);
                                _logger->debug("[Handler] [{}] (G.1.2.1.1.1) Create REP message towards L and send it.",
                                               display(_uuid));
                            }

                            // (G.1.2.1.1.2(bis)) Begin timer for REP timeout.
                            _timeout_handler->initiate_timeout(this, TimeoutType::REP);
                            _logger->debug("[Handler] [{}] (G.1.2.1.1.2(bis)) Begin timer for REP timeout.",
                                           display(_uuid));
                            // TODO _timeout_handler->initiate_timeout(this, RANK_REP_TO_REP_TIMEOUT);

                            // Change state to CLOSED.
                            //old_state = _state;
                            //_state = HandlerState::CLOSED;
                            //_logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid), handler_state_to_string(old_state),
                            //               handler_state_to_string(_state));

                            // (G.1.2.1.1.2) Terminate thread.
                            stop();
                            _logger->debug("[Handler] [{}] (G.1.2.1.1.2) Terminate thread.", display(_uuid));
                            break;

                            /* TODO Pass this to the callback instead.
                            // (G.1.2.1.1.3) Is UUID in the TranslationTable?
                            _logger->trace("[Handler] [{}] (G.1.2.1.1.3) Is UUID in the TranslationTable?", display(_uuid));
                            UUIDv4 original_uuid = _uuid;
                            if (!is_translation_table_empty_for(_uuid)) {
                                original_uuid = locate_original_of(_uuid);

                                // (G.1.2.1.2.3.1.1) Delete UUID* in Store.
                                _logger->trace("[Handler] [{}] (G.1.2.1.2.3.1.1) Delete UUID* in Store.", display(_uuid));
                                // TODO
                            }

                            // (G.1.2.1.2.4) Create REF message with original UUID and send it. TODO
                            _logger->trace("[Handler] [{}] (G.1.2.1.2.4) Create REF message with original UUID and send it.", display(_uuid));
                            REF* new_ref_message = new REF(original_uuid);
                            _dispatcher->send_message(new_ref_message, _source_identifier.first, _source_identifier.second);

                            // Change state to CLOSED.
                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid), handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (G.1.2.1.2.5) Delete UUID from the Store.
                            _logger->trace("[Handler] [{}] (G.1.2.1.2.5) Delete UUID from the Store.", display(_uuid));
                            stop();
                            break;*/
                        } else {
                            _logger->debug("[Handler] [{}] (G.1.2.1) Is UUID in Store? No.", display(_uuid));

                            // (G.1.2.1.2.1) Discard and ignore message.
                            _logger->debug("[Handler] [{}] (G.1.2.1.2.1) Discard and ignore message.", display(_uuid));

                            // Change state to CLOSED.
                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->trace("[Handler] [{}] Handler state transitioned from {} to {}.", display(_uuid),
                                           handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (G.1.2.1.2.2) Terminate thread.
                            stop();
                            _logger->debug("[Handler] [{}] (G.1.2.1.2.2) Terminate thread.");
                            break;
                        }
                    }
                }
                    break;
                case MessageType::NOTYPE:
                    _logger->error("[Handler] [{}] An erroneous message was received. It will be ignored.",
                                   display(_uuid));
                    break;
            }

            _logger->debug("[Handler] [{}] The message is being deleted from the system...", display(_uuid));

            // Delete the pointer of the message.
            delete _message;
            _message = nullptr;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }

    // Closing data before thread death.
}

Handler *Handler::log_on(const std::string &logger_name) {
    _logger = spdlog::get(logger_name);

    return this;
}
