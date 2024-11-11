#include "structs/handler.h"

Handler::Handler(Resources* resources, TranslationTable* translation_table, std::mutex* translation_table_mutex,
                 TimeoutHandler* timeout_handler, Store* store, std::mutex* store_mutex, const UUIDv4& uuid) {
    _logger->trace("[Handler] [{}] Creating handler for this UUID...", uuid);

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
    _logger->trace("[Handler] [{}] The handler was successfully created and given the state of {}.", uuid,
                   handler_state_to_string(_state));
    _logger->info("A handler was created for the {} session.", uuid);
}

Handler::Handler(Resources* resources, TranslationTable* translation_table, std::mutex* translation_table_mutex,
                 TimeoutHandler* timeout_handler, Store* store, std::mutex* store_mutex, const Header& header) {
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

Handler* Handler::handle(Message* message) {
    // Update the message pointer.
    _message = message;

    return this;
}

UUIDv4 Handler::create_translation(const UUIDv4& original) {
    // Generate a new UUID.
    UUIDv4 new_uuid = generate_new_uuid();

    // Save translation in the TranslationTable.
    {
        std::lock_guard<std::mutex> lock(*_translation_table_locker);
        _translation_table->insert({new_uuid, original});
    }

    // Return the new UUID.
    return new_uuid;
}

UUIDv4 Handler::locate_original_of(const UUIDv4& translated) {
    // If there is no such thing as the UUID in translated, in TranslationTable, return 0.
    if (is_translation_table_empty_for(translated)) {
        return 0;
    }

    // Otherwise, safely return the value in the TranslationTable.
    std::lock_guard<std::mutex> lock(*_translation_table_locker);
    return _translation_table->at(translated);
}

bool Handler::is_translation_table_empty_for(const UUIDv4& uuid) {
    std::lock_guard<std::mutex> lock(*_translation_table_locker);
    return !_translation_table->contains(uuid);
}

bool Handler::is_translation_table_empty() {
    std::lock_guard<std::mutex> lock(*_translation_table_locker);
    return _translation_table->empty();
}

void Handler::new_bid(float bid, std::array<uint8_t, 4>& ipv4_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::make_pair(std::vector<uint8_t>(ipv4_address.begin(), ipv4_address.end()), IdentifierType::IPv4)));
}

void Handler::new_bid(float bid, std::array<uint8_t, 6>& mac_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::make_pair(std::vector<uint8_t>(mac_address.begin(), mac_address.end()), IdentifierType::MAC)));
}

void Handler::new_bid(float bid, std::array<uint8_t, 16>& ipv6_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::make_pair(std::vector<uint8_t>(ipv6_address.begin(), ipv6_address.end()), IdentifierType::IPv6)));
}

void Handler::new_bid(float bid, std::string& dds_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::make_pair(std::vector<uint8_t>(dds_address.begin(), dds_address.end()), IdentifierType::DDS)));
}

void Handler::clear_bids() {
    _bids.clear();
}

size_t Handler::cardinal_bids() {
    return _bids.size();
}

std::set<std::pair<std::vector<uint8_t>, IdentifierType>> Handler::min_bids() {
    // Copy the bids set to a safe-to-modify variable.
    BidSet bids_copy {};
    {
        std::lock_guard<std::mutex> lock(_bids_locker);
        bids_copy = _bids;
    }

    // Assume the first item in bids is the minimum one.
    auto minimum_bid_item = bids_copy.begin();
    bids_copy.erase(minimum_bid_item);
    auto minimum_bid = minimum_bid_item->first;

    // Create the set to be returned.
    std::set<std::pair<std::vector<uint8_t>, IdentifierType>> minimum_bids {};

    // Iterate over all the given bids and attempt to find smaller than current minimum and all equal bids.
    for (const auto& bid : bids_copy) {
        if (bid.first < minimum_bid) {
            minimum_bid = bid.first;
            minimum_bids.clear();
            minimum_bids.insert(std::make_pair(bid.second.first, bid.second.second));
        } else if (bid.first == minimum_bid) {
            minimum_bids.insert(std::make_pair(bid.second.first, bid.second.second));
        }
    }

    // Return the minimum bids new targets set.
    return minimum_bids;
}

bool Handler::is_min_bid_unique(const std::set<std::pair<std::vector<uint8_t>, IdentifierType>>& targets) const {
    return targets.size() == 1;
}

bool Handler::is_bid_in_store(const UUIDv4& id) const {
    // If the store does not have the UUID, return false; otherwise check if pre-reserved.
    if (!is_uuid_in_store(id)) {
        return false;
    } else {
        return _store->at(id)->state() == HandlerState::PRE_RESERVED;
    }
}

bool Handler::is_uuid_in_store(const UUIDv4& id) const {
    return _store->contains(id);
}

void Handler::produce_reservation(const RequestingCapabilities& capabilities, uint8_t priority,
                                  const std::vector<uint8_t>& listener) {
    // Create a new reservation object via a pointer.
    _reservation = new Reservation(capabilities, priority);

    // Mark listener as the listener of this reservation.
    _reservation->mark_listener(listener);
}

UUIDv4 Handler::id() const {
    return _uuid;
}

HandlerState Handler::state() const {
    return _state;
}

Reservation* Handler::associated_reservation() const {
    return _reservation;
}

Handler *Handler::borrow(Dispatcher *dispatcher) {
    _dispatcher = dispatcher;

    return this;
}

#ifdef FROM_SIMUZILLA
Handler *Handler::borrow(std::function<std::set<uint8_t>(uint8_t)> function) {
    _get_connections_to = std::move(function);

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

Handler* Handler::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

Handler* Handler::stop() {
    if (!_running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool Handler::is_running() const {
    return _running;
}

void Handler::operator()() {
    _logger->trace("[Handler] [{}] This handler session is starting/resuming functions.", _uuid);

    // Variables for debugging purposes.
    HandlerState old_state{};

    // Initialization for the threading mechanism.

    while (_running) {
        if (_message != nullptr) {
            // Parse message to their types and handle the parsing algorithm.
            switch (_message->type()) {
                case MessageType::EAR: {
                    _logger->trace("[Handler] [{}] The received message is an EAR message.", _uuid);
                    auto ear_message = dynamic_cast<EAR*>(_message);

                    // As this node is receiving a new EAR message, then change state to ASSESSING.
                    old_state = _state;
                    _state = HandlerState::ASSESSING;
                    _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                   handler_state_to_string(_state));

                    // (B.1) Check if the current node is the message's listener.
                    _logger->trace("[Handler] [{}] (B.1) Am I the listener?", _uuid);
                    auto listener_field = ear_message->listener();
                    auto listener_field_length = ear_message->listener_length();
                    bool i_am_listener = false;
                    switch (listener_field_length) {
                        case RANK_EAR_MESSAGE_LEN_LT_IP4: {
                            std::array<uint8_t, 4> ip4_address {};
                            for (int byte = 0; byte != 4; byte++) {
                                ip4_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(ip4_address);

                            // Update inner reservation with the requirements of the message.
                            produce_reservation(ear_message->requirements(), ear_message->priority(),
                                                std::vector<uint8_t>(ip4_address.begin(), ip4_address.end()));
                        } break;
                        case RANK_EAR_MESSAGE_LEN_LT_MAC: {
                            std::array<uint8_t, 6> mac_address {};
                            for (int byte = 0; byte != 6; byte++) {
                                mac_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(mac_address);

                            // Update inner reservation with the requirements of the message.
                            produce_reservation(ear_message->requirements(), ear_message->priority(),
                                                std::vector<uint8_t>(mac_address.begin(), mac_address.end()));
                        } break;
                        case RANK_EAR_MESSAGE_LEN_LT_IP6: {
                            std::array<uint8_t, 16> ip6_address {};
                            for (int byte = 0; byte != 16; byte++) {
                                ip6_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(ip6_address);

                            // Update inner reservation with the requirements of the message.
                            produce_reservation(ear_message->requirements(), ear_message->priority(),
                                                std::vector<uint8_t>(ip6_address.begin(), ip6_address.end()));
                        } break;
                        default:
                            throw std::exception();  // TODO
                    }

                    if (i_am_listener) {
                        // (B.1.1.1) Can R be performed with priority p?
                        _logger->trace("[Handler] [{}] (B.1.1.1) Can R be performed with priority p?", _uuid);
                        auto* position = _resources->available_for_performance(*_reservation, ear_message->priority());
                        if (position != nullptr) {
                            // (B.1.1.1.1.1) Reserve R with UUID in Store.
                            _logger->trace("[Handler] [{}] (B.1.1.1.1.1) Reserve R with UUID in Store.", _uuid);
                            _resources->mark_reservation(_reservation);

                            // (B.1.1.1.1.2) Create an ACC message and send it.
                            _logger->trace("[Handler] [{}] (B.1.1.1.1.2) Create an ACC message and send it back.", _uuid);
                            ACC* acc_message = new ACC(_uuid);
                            _dispatcher->send_message(acc_message, _source_identifier.first, _source_identifier.second);

                            // (B.1.1.1.1.3) Change state to RESERVED and terminate thread.
                            _logger->trace("[Handler] [{}] (B.1.1.1.1.3) Change state to RESERVED and terminate thread.", _uuid);
                            old_state = _state;
                            _state = HandlerState::RESERVED;
                            _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                           handler_state_to_string(_state));
                            stop();
                            break;
                        } else {
                            // (B.1.1.2) Create a REF message and send it.
                            _logger->trace("[Handler] [{}] (B.1.1.2) Create a REF message and send it back.", _uuid);
                            REF* ref_message = new REF(_uuid);
                            _dispatcher->send_message(ref_message, _source_identifier.first, _source_identifier.second);

                            // (B.1.1.3) Change state to CLOSED and delete UUID from the Store.
                            _logger->trace("[Handler] [{}] (B.1.1.3) Change state to CLOSED and delete UUID from the Store.", _uuid);
                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                           handler_state_to_string(_state));
                            stop();
                            return;
                        }
                    } else {
                        // (B.1.2.1) Is there a bid in Store for the UUID?
                        _logger->trace("[Handler] [{}] (B.1.2.1) Is there a bid in Store for the UUID?", _uuid);
                        if (!is_bid_in_store(ear_message->uuid())) {
                            // (B.1.2.1.1.1) Can R be performed with priority p?
                            _logger->trace("[Handler] [{}] (B.1.2.1.1.1) Can R be performed with priority p?", _uuid);
                            auto* position =
                                    _resources->available_for_performance(*_reservation, ear_message->priority());
                            if (position != nullptr) {
                                // (B.1.2.1.1.2) Pre-reserve R with UUID in Store.
                                _logger->trace("[Handler] [{}] (B.1.2.1.1.2) Pre-reserve R with UUID in Store.", _uuid);
                                _resources->mark_pre_reservation(_reservation);

                                // (B.1.2.1.2.1) Get connections towards listener(s).
                                _logger->trace("[Handler] [{}] (B.1.2.1.2.1) Get connections towards listener(s).", _uuid);
                                std::vector<uint8_t> target {};
                                switch (listener_field_length) {
                                    case RANK_EAR_MESSAGE_LEN_LT_IP4: {
                                        for (int byte = 0; byte != 4; byte++) {
                                            target.push_back(listener_field[byte]);
                                        }
                                    } break;
                                    case RANK_EAR_MESSAGE_LEN_LT_MAC: {
                                        for (int byte = 0; byte != 6; byte++) {
                                            target.push_back(listener_field[byte]);
                                        }
                                    } break;
                                    case RANK_EAR_MESSAGE_LEN_LT_IP6: {
                                        for (int byte = 0; byte != 16; byte++) {
                                            target.push_back(listener_field[byte]);
                                        }
                                    } break;
                                    default:
                                        throw std::exception();  // TODO
                                }
#ifdef FROM_SIMUZILLA
                                std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target =
                                        get_connections_to(target.at(0));
#else
                                std::vector<std::pair<std::vector<uint8_t>, IdentifierType>> connections_to_target =
                                        get_connections_to(target);
#endif

                                // (B.1.2.2) Depending on the cardinal of connections...
                                _logger->trace("[Handler] [{}] (B.1.2.2) Depending on the cardinal of connections...", _uuid);
                                switch (connections_cardinal(connections_to_target)) {
                                    case 0: {
                                        // If no connection is found...
                                        // (B.1.2.2.3.1) Create REF message and send it.
                                        _logger->trace("[Handler] [{}] (B.1.2.2.3.1) If no connection is found... create REF message and send it back.", _uuid);
                                        REF* ref_message = new REF(_uuid);
                                        _dispatcher->send_message(ref_message, _source_identifier.first, _source_identifier.second);

                                        // (B.1.2.2.3.2) Change state to CLOSED and delete UUID from the Store.
                                        _logger->trace("[Handler] [{}] (B.1.2.2.3.2) Change state to CLOSED and delete UUID from the Store.", _uuid);
                                        old_state = _state;
                                        _state = HandlerState::CLOSED;
                                        _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                                                                     handler_state_to_string(_state));
                                        stop();
                                        return;
                                    } break;
                                    case 1: {
                                        // If only one connection is found...
                                        // (B.1.2.2.2.1) Create EAR message and send it.
                                        _logger->trace("[Handler] [{}] (B.1.2.2.2.1) If only one connection is found... create EAR message and send it.", _uuid);
                                        EAR* new_ear_message =
                                                new EAR(_uuid, ear_message->priority(), ear_message->listener_length(),
                                                    listener_message_format(connections_to_target.front().first), ear_message->payload_length(),
                                                    ear_message->payload());
                                        _dispatcher->send_message(new_ear_message, connections_to_target.front().first, connections_to_target.front().second);

                                        // Change state to PRE_RESERVED.
                                        old_state = _state;
                                        _state = HandlerState::PRE_RESERVED;
                                        _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                                       handler_state_to_string(_state));

                                        // (B.1.2.2.2.2) Add message source address as reservation past node.
                                        _logger->trace("[Handler] [{}] (B.1.2.2.2.2) Add message source address as reservation past node.", _uuid);
                                        _reservation->set_past_node(_source_identifier);

                                        // (B.1.2.2.2.3) Begin timer for EAR timeout.
                                        _logger->trace("[Handler] [{}] (B.1.2.2.2.3) Begin timer for EAR timeout.", _uuid);
                                        _timeout_handler->initiate_timeout(this, RANK_EAR_TO_EAR_TIMEOUT);
                                    } break;
                                    default: {
                                        // If more than one connection is found...
                                        // (B.1.2.2.1.1) For each connection create a MAR message and send it.
                                        _logger->trace("[Handler] [{}] (B.1.2.2.1.1) If more than one connection is found... for each connection create a MAR message and send it.", _uuid);
                                        for (const auto& intermediate : connections_to_target) {
                                            MAR* mar_message =
                                                    new MAR(_uuid, ear_message->priority(), ear_message->listener_length(),
                                                        listener_message_format(intermediate.first),
                                                        ear_message->payload_length(), ear_message->payload());
                                            _dispatcher->send_message(mar_message, intermediate.first, intermediate.second);
                                        }

                                        // Change state to PRE_RESERVED.
                                        old_state = _state;
                                        _state = HandlerState::PRE_RESERVED;
                                        _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                                       handler_state_to_string(_state));

                                        // (B.1.2.2.1.2) Begin timer for MAR timeout.
                                        _logger->trace("[Handler] [{}] (B.1.2.2.1.2) Begin timer for MAR timeout.", _uuid);
                                        _timeout_handler->initiate_timeout(this, RANK_MAR_TO_MAR_TIMEOUT);
                                    } break;
                                }
                            } else {
                            }
                        } else {
                            // (B.1.1.2) Create a REF message and send it.
                            _logger->trace("[Handler] [{}] (B.1.1.2) Create a REF message and send it back.", _uuid);
                            REF* ref_message = new REF(_uuid);
                            _dispatcher->send_message(ref_message, _source_identifier.first, _source_identifier.second);

                            // (B.1.1.3) Change state to CLOSED and delete UUID from the Store.
                            _logger->trace("[Handler] [{}] (B.1.1.3) Change state to CLOSED and delete UUID from the Store.", _uuid);
                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                           handler_state_to_string(_state));
                            stop();
                            return;
                        }
                    }
                } break;
                case MessageType::MAR: {
                    auto mar_message = dynamic_cast<MAR*>(_message);

                    // Change state to AUCTION_BIDDING.
                    old_state = _state;
                    _state = HandlerState::AUCTION_BIDDING;
                    _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                   handler_state_to_string(_state));

                    auto listener_field = mar_message->listener();
                    auto listener_field_length = mar_message->listener_length();
                    switch (listener_field_length) {
                        case RANK_MAR_MESSAGE_LEN_LT_IP4: {
                            std::array<uint8_t, 4> ip4_address {};
                            for (int byte = 0; byte != 4; byte++) {
                                ip4_address[byte] = listener_field[byte];
                            }

                            // Update inner reservation with the requirements of the message.
                            produce_reservation(mar_message->requirements(), mar_message->priority(),
                                                std::vector<uint8_t>(ip4_address.begin(), ip4_address.end()));
                        } break;
                        case RANK_MAR_MESSAGE_LEN_LT_MAC: {
                            std::array<uint8_t, 6> mac_address {};
                            for (int byte = 0; byte != 6; byte++) {
                                mac_address[byte] = listener_field[byte];
                            }

                            // Update inner reservation with the requirements of the message.
                            produce_reservation(mar_message->requirements(), mar_message->priority(),
                                                std::vector<uint8_t>(mac_address.begin(), mac_address.end()));
                        } break;
                        case RANK_MAR_MESSAGE_LEN_LT_IP6: {
                            std::array<uint8_t, 16> ip6_address {};
                            for (int byte = 0; byte != 16; byte++) {
                                ip6_address[byte] = listener_field[byte];
                            }

                            // Update inner reservation with the requirements of the message.
                            produce_reservation(mar_message->requirements(), mar_message->priority(),
                                                std::vector<uint8_t>(ip6_address.begin(), ip6_address.end()));
                        } break;
                        default:
                            throw std::exception();  // TODO
                    }

                    // (C.1) Can R be performed with priority p?
                    _logger->trace("[Handler] [{}] (C.1) Can R be performed with priority p?", _uuid);
                    auto* position = _resources->available_for_performance(*_reservation, mar_message->priority());
                    if (position != nullptr) {
                        // (C.1.2.1) Estimate bid.
                        _logger->trace("[Handler] [{}] (C.1.2.1) Estimate bid.", _uuid);
                        float bid_value = _resources->estimate_bid(*_reservation);

                        // (C.1.2.2) Create bid message and send it.
                        _logger->trace("[Handler] [{}] (C.1.2.2) Create bid message and send it.", _uuid);
                        BID* bid_message = new BID(_uuid, bid_value);
                        _dispatcher->send_message(bid_message, _source_identifier.first, _source_identifier.second);

                        // Change state to PRE_RESERVED.
                        old_state = _state;
                        _state = HandlerState::PRE_RESERVED;
                        _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (C.1.2.3) Pre-reserve R with UUID in the store.
                        _logger->trace("[Handler] [{}] (C.1.2.3) Pre-reserve R with UUID in the store.", _uuid);
                        _resources->mark_pre_reservation(_reservation);

                        // (C.1.2.4) Begin timer for BID timeout.
                        _logger->trace("[Handler] [{}] (C.1.2.4) Begin timer for BID timeout.", _uuid);
                        _timeout_handler->initiate_timeout(this, RANK_BID_TO_BID_TIMEOUT);
                    } else {
                        // (C.1.1.1) Create zeroed-bid message and send it.
                        _logger->trace("[Handler] [{}] (C.1.1.1) Create zeroed-bid message and send it.", _uuid);
                        BID* bid_message = new BID(_uuid, 0);
                        _dispatcher->send_message(bid_message, _source_identifier.first, _source_identifier.second);

                        // Change handler's state to CLOSED.
                        old_state = _state;
                        _state = HandlerState::CLOSED;
                        _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (C.1.1.2) Delete UUID from the Store.
                        _logger->trace("[Handler] [{}] (C.1.1.2) Delete UUID from the Store.", _uuid);
                        stop();
                        break;
                    }
                } break;
                case MessageType::BID: {
                    // (D.1) Is auction waiting?
                    if (_state != HandlerState::AUCTION_WAITING) {
                        _logger->trace("[Handler] [{}] (D.1) Is auction waiting?", _uuid);
                        _logger->warn("[Handler] [{}] Handler received an unexpected BID message.", _uuid);

                        // (D.1.1.1) Delete UUID from the Store.
                        _logger->trace("[Handler] [{}] (D.1.1.1) Delete UUID from the Store.");
                        stop();
                        return;
                    }

                    auto bid_message = dynamic_cast<BID*>(_message);

                    // (D.1.2.1) Wait for all bids.
                    _logger->trace("[Handler] [{}] (D.1.2.1) Wait for all bids.", _uuid);
                    // TODO Add bids until a timeout is reached (call new_bid methods according to the type).

                    // (D.1.2.3) Check the cardinal of bids.
                    _logger->trace("[Handler] [{}] (D.1.2.3) Check the cardinal of bids.", _uuid);
                    if (cardinal_bids() == 0) {
                        // (D.1.2.3.1.1) Create REF message and send it.
                        _logger->trace("[Handler] [{}] (D.1.2.3.1.1) Create REF message and send it.", _uuid);
                        REF* ref_message = new REF(_uuid);
                        _dispatcher->send_message(ref_message, _source_identifier.first, _source_identifier.second);

                        // Change the state to CLOSED.
                        old_state = _state;
                        _state = HandlerState::CLOSED;
                        _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (D.1.2.3.1.2) Delete UUID from the Store.
                        _logger->trace("[Handler] [{}] (D.1.2.3.1.2) Delete UUID from the Store.", _uuid);
                        stop();
                        break;
                    } else {
                        // (D.1.2.3.2.1) Is the minimum bid unique?
                        _logger->trace("[Handler] [{}] (D.1.2.3.2.1) Is the minimum bid unique?", _uuid);
                        auto minimum_bids = min_bids();
                        if (is_min_bid_unique(minimum_bids)) {
                            // (D.1.2.3.2.1.1.1) As it is unique, send an EAR message to it.
                            _logger->trace("[Handler] [{}] (D.1.2.3.2.1.1.1) As it is unique, send an EAR message to it.", _uuid);
                            EAR* ear_message = new EAR(_uuid, _reservation->priority(), _reservation->listener_length(),
                                                  _reservation->listener(),
                                                  make_payload_length(_reservation->requirements()),
                                                  make_payload(_reservation->requirements()));
                            _dispatcher->send_message(ear_message, minimum_bids.begin()->first, minimum_bids.begin()->second);

                            // Change the state to PRE_RESERVED.
                            old_state = _state;
                            _state = HandlerState::PRE_RESERVED;
                            _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (D.1.2.3.2.1.1.2) Add message source address as reservation past node.
                            _logger->trace("[Handler] [{}] (D.1.2.3.2.1.1.2) Add message source address as reservation past node.", _uuid);
                            _reservation->set_past_node(_source_identifier);

                            // (D.1.2.3.2.1.1.3) Begin timer for EAR timeout.
                            _logger->trace("[Handler] [{}] (D.1.2.3.2.1.1.3) Begin timer for EAR timeout.", _uuid);
                            _timeout_handler->initiate_timeout(this, RANK_EAR_TO_EAR_TIMEOUT);

                            // (D.1.2.3.2.1.1.4) Delete UUID from the Store.
                            _logger->trace("[Handler] [{}] (D.1.2.3.2.1.1.4) Delete UUID from the Store.", _uuid);
                            stop();
                            break;
                        } else {
                            // As it is not unique...
                            for (const auto& bid_target : _bids) {
                                // (D.1.2.3.2.1.2.1) Create a new UUID for each min(B) node and (D.3.2.1.2.2) Save
                                // translations UUID to UUID* in TranslationTable.
                                _logger->trace("[Handler] [{}] (D.1.2.3.2.1.2.1) Create a new UUID for each min(B) node and (D.1.2.3.2.1.2.2) Save translations UUID to UUID* in TranslationTable.", _uuid);
                                auto new_uuid = create_translation(_uuid);

                                // (D.1.2.3.2.1.2.3) Create EAR message and send it to each min(B) node, with UUID*.
                                _logger->trace("[Handler] [{}] (D.1.2.3.2.1.2.3) Create EAR message and send it to each min(B) node, with UUID*.", _uuid);
                                EAR* ear_message = new EAR(new_uuid, _reservation->priority(),
                                                      _reservation->listener_length(), _reservation->listener(),
                                                      make_payload_length(_reservation->requirements()),
                                                      make_payload(_reservation->requirements()));
                                _dispatcher->send_message(ear_message, bid_target.second.first, bid_target.second.second);

                                // Change the state to PRE_RESERVED.
                                old_state = _state;
                                _state = HandlerState::PRE_RESERVED;
                                _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                               handler_state_to_string(_state));

                                // (D.1.2.3.2.1.2.4) Add message source address as reservation past node.
                                _logger->trace("[Handler] [{}] (D.1.2.3.2.1.2.4) Add message source address as reservation past node.", _uuid);
                                _reservation->set_past_node(_source_identifier);

                                // (D.1.2.3.2.1.2.5) Begin timer for EAR timeout.
                                _logger->trace("[Handler] [{}] (D.1.2.3.2.1.2.5) Begin timer for EAR timeout.", _uuid);
                                _timeout_handler->initiate_timeout(this, RANK_EAR_TO_EAR_TIMEOUT);

                                // (D.1.2.3.2.1.2.6) Delete UUID from the Store.
                                _logger->trace("[Handler] [{}] (D.1.2.3.2.1.2.6) Delete UUID from the Store.", _uuid);
                                stop();
                                break;
                            }
                        }
                    }
                } break;
                case MessageType::ACC: {
                    auto acc_message = dynamic_cast<ACC*>(_message);

                    // (E.1) Set pre-reservation as reserved on UUID in Store.
                    _logger->trace("[Handler] [{}] (E.1) Set pre-reservation as reserved on UUID in Store.", _uuid);
                    _resources->mark_reservation(_reservation);

                    // (E.2) Is UUID in the TranslationTable?
                    _logger->trace("[Handler] [{}] (E.2) Is UUID in the TranslationTable?", _uuid);
                    if (!is_translation_table_empty_for(_uuid)) {
                        UUIDv4 original_uuid = locate_original_of(_uuid);

                        // (E.2.2.1) Create ACC message to the original UUID.
                        _logger->trace("[Handler] [{}] (E.2.2.1) Create ACC message to the original UUID.", _uuid);
                        ACC* new_acc_message = new ACC(original_uuid);
                        _dispatcher->send_message(new_acc_message, _source_identifier.first, _source_identifier.second);

                        // Change state to RESERVED.
                        old_state = _state;
                        _state = HandlerState::RESERVED;
                        _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (E.2.2.2) Add accepting node as next node of the reservation.
                        _logger->trace("[Handler] [{}] (E.2.2.2) Add accepting node as next node of the reservation.", _uuid);
                        _reservation->add_next_node(_accepting_nodes.back());

                        // (E.2.2.3) Terminate the thread.
                        _logger->trace("[Handler] [{}] (E.2.2.3) Terminate the thread.", _uuid);
                        stop();
                        break;
                    } else {
                        // (E.2.1.1) Create ACC message to the UUID.
                        _logger->trace("[Handler] [{}] (E.2.1.1) Create ACC message to the UUID.", _uuid);
                        ACC* new_acc_message = new ACC(_uuid);
                        _dispatcher->send_message(new_acc_message, _source_identifier.first, _source_identifier.second);

                        // Change state to RESERVED.
                        old_state = _state;
                        _state = HandlerState::RESERVED;
                        _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (E.2.1.2) Add accepting node as next node of the reservation.
                        _logger->trace("[Handler] [{}] (E.2.1.2) Add accepting node as next node of the reservation.", _uuid);
                        _reservation->add_next_node(_accepting_nodes.back());

                        // (E.2.1.3) Terminate the thread.
                        _logger->trace("[Handler] [{}] (E.2.1.3) Terminate the thread.", _uuid);
                        stop();
                        break;
                    }
                } break;
                case MessageType::REF: {
                    auto ref_message = dynamic_cast<REF*>(_message);

                    // (F.1) Delete pre-reservations with UUID in the Store.
                    _logger->trace("[Handler] [{}] (F.1) Delete pre-reservations with UUID in the Store.", _uuid);
                    // TODO

                    // (F.2) Is UUID in the TranslationTable?
                    _logger->trace("[Handler] [{}] (F.2) Is UUID in the TranslationTable?", _uuid);
                    if (!is_translation_table_empty_for(_uuid)) {
                        // (F.2.1.1) Delete UUID* in Store.
                        _logger->trace("[Handler] [{}] (F.2.1.1) Delete UUID* in Store.", _uuid);
                        // TODO

                        // (F.2.1.2) Is TranslationTable empty?
                        _logger->trace("[Handler] [{}] (F.2.1.2) Is TranslationTable empty?", _uuid);
                        if (is_translation_table_empty()) {
                            // (F.3) Create REF message and send it.
                            _logger->trace("[Handler] [{}] (F.3) Create REF message and send it.", _uuid);
                            REF* new_ref_message = new REF(_uuid);
                            _dispatcher->send_message(new_ref_message, _source_identifier.first, _source_identifier.second);
                        } else {
                            stop();
                            break;
                        }
                    } else {
                        // (F.3) Create REF message and send it.
                        _logger->trace("[Handler] [{}] (F.3) Create REF message and send it.", _uuid);
                        REF* new_ref_message = new REF(_uuid);
                        _dispatcher->send_message(new_ref_message, _source_identifier.first, _source_identifier.second);
                    }

                    // Change state to CLOSED.
                    old_state = _state;
                    _state = HandlerState::CLOSED;
                    _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                   handler_state_to_string(_state));

                    // (F.4) Delete UUID from the Store.
                    _logger->trace("[Handler] [{}] (F.4) Delete UUID from the Store.", _uuid);
                    stop();
                    break;
                } break;
                case MessageType::REP: {
                    auto rep_message = dynamic_cast<REP*>(_message);

                    // Change state to REPLENISHING.
                    old_state = _state;
                    _state = HandlerState::REPLENISHING;
                    _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                   handler_state_to_string(_state));

                    // (G.1) Check if the current node is the message's listener.
                    _logger->trace("[Handler] [{}] (G.1) Check if the current node is the message's listener.", _uuid);
                    auto listener_field = rep_message->listener();
                    auto listener_field_length = rep_message->listener_length();
                    bool i_am_listener = false;
                    switch (listener_field_length) {
                        case RANK_EAR_MESSAGE_LEN_LT_IP4: {
                            std::array<uint8_t, 4> ip4_address {};
                            for (int byte = 0; byte != 4; byte++) {
                                ip4_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(ip4_address);
                        } break;
                        case RANK_EAR_MESSAGE_LEN_LT_MAC: {
                            std::array<uint8_t, 6> mac_address {};
                            for (int byte = 0; byte != 6; byte++) {
                                mac_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(mac_address);
                        } break;
                        case RANK_EAR_MESSAGE_LEN_LT_IP6: {
                            std::array<uint8_t, 16> ip6_address {};
                            for (int byte = 0; byte != 16; byte++) {
                                ip6_address[byte] = listener_field[byte];
                            }
                            i_am_listener = is_me(ip6_address);
                        } break;
                        default:
                            throw std::exception();  // TODO
                    }
                    if (i_am_listener) {
                        // (G.1.1.1) Create REF message and send it.
                        _logger->trace("[Handler] [{}] (G.1.1.1) Create REF message and send it.", _uuid);
                        REF* ref_message = new REF(_uuid);
                        _dispatcher->send_message(ref_message, _source_identifier.first, _source_identifier.second);

                        // Change state to CLOSED.
                        old_state = _state;
                        _state = HandlerState::CLOSED;
                        _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                       handler_state_to_string(_state));

                        // (G.1.1.2) Delete UUID from the Store.
                        _logger->trace("[Handler] [{}] (G.1.1.2) Delete UUID from the Store.", _uuid);
                        stop();
                        break;
                    } else {
                        // (G.1.2.1) Is UUID in Store?
                        _logger->trace("[Handler] [{}] (G.1.2.1) Is UUID in Store?", _uuid);
                        if (is_uuid_in_store(_uuid)) {
                            // (G.1.2.1.2.1) Create REP message towards L and send it.
                            _logger->trace("[Handler] [{}] (G.1.2.1.2.1) Create REP message towards L and send it.", _uuid);
                            REP* new_rep_message = new REP(_uuid, rep_message->listener_length(), rep_message->listener());
                            for (const auto& direction : _reservation->next_nodes()) {
                                _dispatcher->send_message(new_rep_message, direction.first, direction.second);
                            }

                            // (G.1.2.1.2.2) Begin timer for REP timeout.
                            _logger->trace("[Handler] [{}] (G.1.2.1.2.2) Begin timer for REP timeout.", _uuid);
                            _timeout_handler->initiate_timeout(this, RANK_REP_TO_REP_TIMEOUT);

                            // (G.1.2.1.2.3) Is UUID in the TranslationTable?
                            _logger->trace("[Handler] [{}] (G.1.2.1.2.3) Is UUID in the TranslationTable?", _uuid);
                            UUIDv4 original_uuid = _uuid;
                            if (!is_translation_table_empty_for(_uuid)) {
                                original_uuid = locate_original_of(_uuid);

                                // (G.1.2.1.2.3.1.1) Delete UUID* in Store.
                                _logger->trace("[Handler] [{}] (G.1.2.1.2.3.1.1) Delete UUID* in Store.", _uuid);
                                // TODO
                            }

                            // (G.1.2.1.2.4) Create REF message with original UUID and send it.
                            _logger->trace("[Handler] [{}] (G.1.2.1.2.4) Create REF message with original UUID and send it.", _uuid);
                            REF* new_ref_message = new REF(original_uuid);
                            _dispatcher->send_message(new_ref_message, _source_identifier.first, _source_identifier.second);

                            // Change state to CLOSED.
                            old_state = _state;
                            _state = HandlerState::CLOSED;
                            _logger->debug("[Handler] [{}] Handler state transitioned from {} to {}.", _uuid, handler_state_to_string(old_state),
                                           handler_state_to_string(_state));

                            // (G.1.2.1.2.5) Delete UUID from the Store.
                            _logger->trace("[Handler] [{}] (G.1.2.1.2.5) Delete UUID from the Store.", _uuid);
                            stop();
                            break;
                        } else {
                            // (G.1.2.1.1.1) Discard and ignore message.
                            _logger->trace("[Handler] [{}] (G.1.2.1.1.1) Discard and ignore message.", _uuid);
                            stop();
                            break;
                        }
                    }
                } break;
                case MessageType::NOTYPE:
                    _logger->error("[Handler] [{}] An erroneous message was received. It will be ignored.", _uuid);
                    break;
            }

            // Delete the pointer of the message.
            delete _message;
            _message = nullptr;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }

    // Closing data before thread death.
}
