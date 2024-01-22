#include "structs/handler.h"

Handler::Handler(Resources* resources, TranslationTable* translation_table, std::mutex* translation_table_mutex,
                 TimeoutHandler* timeout_handler, Store* store, std::mutex* store_mutex, const UUIDv4& uuid) {
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
    _bids.insert(std::make_pair(bid, std::vector<uint8_t>(ipv4_address.begin(), ipv4_address.end())));
}

void Handler::new_bid(float bid, std::array<uint8_t, 6>& mac_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::vector<uint8_t>(mac_address.begin(), mac_address.end())));
}

void Handler::new_bid(float bid, std::array<uint8_t, 16>& ipv6_address) {
    // Safely add the bid to the set of bids.
    std::lock_guard<std::mutex> lock(_bids_locker);
    _bids.insert(std::make_pair(bid, std::vector<uint8_t>(ipv6_address.begin(), ipv6_address.end())));
}

void Handler::clear_bids() {
    _bids.clear();
}

size_t Handler::cardinal_bids() {
    return _bids.size();
}

std::set<std::vector<uint8_t>> Handler::min_bids() {
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
    std::set<std::vector<uint8_t>> minimum_bids {};

    // Iterate over all the given bids and attempt to find smaller than current minimum and all equal bids.
    for (const auto& bid : bids_copy) {
        if (bid.first < minimum_bid) {
            minimum_bid = bid.first;
            minimum_bids.clear();
            minimum_bids.insert(bid.second);
        } else if (bid.first == minimum_bid) {
            minimum_bids.insert(bid.second);
        }
    }

    // Return the minimum bids new targets set.
    return minimum_bids;
}

bool Handler::is_min_bid_unique(const std::set<std::vector<uint8_t>>& targets) const {
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
    // Initialization for the threading mechanism.

    while (_running) {
        if (_message != nullptr) {
            // Parse message to their types and handle the parsing algorithm.
            switch (_message->type()) {
                case MessageType::EAR: {
                    auto ear_message = dynamic_cast<EAR*>(_message);

                    // As this node is receiving a new EAR message, then change state to ASSESSING.
                    _state = HandlerState::ASSESSING;

                    // (B.1) Check if the current node is the message's listener.
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
                        auto* position = _resources->available_for_performance(*_reservation, ear_message->priority());
                        if (position != nullptr) {
                            // (B.1.1.1.1.1) Reserve R with UUID in Store.
                            _resources->mark_reservation(_reservation);

                            // (B.1.1.1.1.2) Create an ACC message and send it.
                            ACC acc_message = ACC(_uuid);
                            // TODO Send message through process-created socket?

                            // (B.1.1.1.1.3) Change state to RESERVED and terminate thread.
                            _state = HandlerState::RESERVED;
                            stop();
                            break;
                        } else {
                            // (B.1.1.2) Create a REF message and send it.
                            REF ref_message = REF(_uuid);
                            // TODO Send message through process-created socket?

                            // (B.1.1.3) Change state to CLOSED and delete UUID from the Store.
                            _state = HandlerState::CLOSED;
                            stop();
                            return;
                        }
                    } else {
                        // (B.1.2.1) Is there a bid in Store for the UUID?
                        if (!is_bid_in_store(ear_message->uuid())) {
                            // (B.1.2.1.1.1) Can R be performed with priority p?
                            auto* position =
                                    _resources->available_for_performance(*_reservation, ear_message->priority());
                            if (position != nullptr) {
                                // (B.1.2.1.1.2) Pre-reserve R with UUID in Store.
                                _resources->mark_pre_reservation(_reservation);

                                // (B.1.2.1.2.1) Get connections towards listener(s).
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
                                std::vector<std::vector<uint8_t>> connections_to_target =
                                        get_connections_to(target);

                                // (B.1.2.2) Depending on the cardinal of connections...
                                switch (connections_cardinal(connections_to_target)) {
                                    case 0: {
                                        // If no connection is found...
                                        // (B.1.2.2.3.1) Create REF message and send it.
                                        REF ref_message = REF(_uuid);
                                        // TODO Send message through process-created socket?

                                        // (B.1.2.2.3.2) Change state to CLOSED and delete UUID from the Store.
                                        _state = HandlerState::CLOSED;
                                        stop();
                                        return;
                                    } break;
                                    case 1: {
                                        // If only one connection is found...
                                        // (B.1.2.2.2.1) Create EAR message and send it.
                                        EAR new_ear_message =
                                                EAR(_uuid, ear_message->priority(), ear_message->listener_length(),
                                                    connections_to_target.front().data(), ear_message->payload_length(),
                                                    ear_message->payload());
                                        // TODO Send message through process-created socket?

                                        // Change state to PRE_RESERVED.
                                        _state = HandlerState::PRE_RESERVED;

                                        // (B.1.2.2.2.2) Begin timer for EAR timeout.
                                        _timeout_handler->initiate_timeout(this, RANK_EAR_TO_EAR_TIMEOUT);
                                    } break;
                                    default: {
                                        // If more than one connection is found...
                                        // (B.1.2.2.1.1) For each connection create a MAR message and send it.
                                        for (const auto& intermediate : connections_to_target) {
                                            MAR mar_message =
                                                    MAR(_uuid, ear_message->priority(), ear_message->listener_length(),
                                                        const_cast<uint8_t*>(intermediate.data()),
                                                        ear_message->payload_length(), ear_message->payload());
                                            // TODO Send message through process-created socket?
                                        }

                                        // Change state to PRE_RESERVED.
                                        _state = HandlerState::PRE_RESERVED;

                                        // (B.1.2.2.1.2) Begin timer for MAR timeout.
                                        _timeout_handler->initiate_timeout(this, RANK_MAR_TO_MAR_TIMEOUT);
                                    } break;
                                }
                            } else {
                            }
                        } else {
                            // (B.1.1.2) Create a REF message and send it.
                            REF ref_message = REF(_uuid);
                            // TODO Send message through process-created socket?

                            // (B.1.1.3) Change state to CLOSED and delete UUID from the Store.
                            _state = HandlerState::CLOSED;
                            stop();
                            return;
                        }
                    }
                } break;
                case MessageType::MAR: {
                    auto mar_message = dynamic_cast<MAR*>(_message);

                    // Change state to AUCTION_BIDDING.
                    _state = HandlerState::AUCTION_BIDDING;

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
                    auto* position = _resources->available_for_performance(*_reservation, mar_message->priority());
                    if (position != nullptr) {
                        // (C.1.2.1) Estimate bid.
                        float bid_value = _resources->estimate_bid(*_reservation);

                        // (C.1.2.2) Create bid message and send it.
                        BID bid_message = BID(_uuid, bid_value);
                        // TODO Send message through process-created socket?

                        // Change state to PRE_RESERVED.
                        _state = HandlerState::PRE_RESERVED;

                        // (C.1.2.3) Pre-reserve R with UUID in the store.
                        _resources->mark_pre_reservation(_reservation);

                        // (C.1.2.4) Begin timer for BID timeout.
                        _timeout_handler->initiate_timeout(this, RANK_BID_TO_BID_TIMEOUT);
                    } else {
                        // (C.1.1.1) Create zeroed-bid message and send it.
                        BID bid_message = BID(_uuid, 0);
                        // TODO Send message through process-created socket?

                        // Change handler's state to CLOSED.
                        _state = HandlerState::CLOSED;

                        // (C.1.1.2) Delete UUID from the Store.
                        stop();
                        break;
                    }
                } break;
                case MessageType::BID: {
                    auto bid_message = dynamic_cast<BID*>(_message);

                    // (D.1) Wait for all bids.
                    // TODO

                    // (D.3) Check the cardinal of bids.
                    if (cardinal_bids() == 0) {
                        // (D.3.1.1) Create REF message and send it.
                        REF ref_message = REF(_uuid);
                        // TODO Send the message.

                        // Change the state to CLOSED.
                        _state = HandlerState::CLOSED;

                        // (D.3.1.2) Delete UUID from the Store.
                        stop();
                        break;
                    } else {
                        // (D.3.2.1) Is the minimum bid unique?
                        if (is_min_bid_unique(min_bids())) {
                            // (D.3.2.1.1.1) As it is unique, send an EAR message to it.
                            EAR ear_message = EAR(_uuid, _reservation->priority(), _reservation->listener_length(),
                                                  _reservation->listener(),
                                                  make_payload_length(_reservation->requirements()),
                                                  make_payload(_reservation->requirements()));
                            // TODO Send this message.

                            // Change the state to PRE_RESERVED.
                            _state = HandlerState::PRE_RESERVED;

                            // (D.3.2.1.1.2) Begin timer for EAR timeout.
                            _timeout_handler->initiate_timeout(this, RANK_EAR_TO_EAR_TIMEOUT);

                            // (D.3.2.1.1.3) Delete UUID from the Store.
                            stop();
                            break;
                        } else {
                            // As it is not unique...
                            for (const auto& bid_target : _bids) {
                                // (D.3.2.1.2.1) Create a new UUID for each min(B) node and (D.3.2.1.2.2) Save
                                // translations UUID to UUID* in TranslationTable.
                                auto new_uuid = create_translation(_uuid);

                                // (D.3.2.1.2.3) Create EAR message and send it to each min(B) node, with UUID*.
                                EAR ear_message = EAR(new_uuid, _reservation->priority(),
                                                      _reservation->listener_length(), _reservation->listener(),
                                                      make_payload_length(_reservation->requirements()),
                                                      make_payload(_reservation->requirements()));
                                // TODO Send this message.

                                // Change the state to PRE_RESERVED.
                                _state = HandlerState::PRE_RESERVED;

                                // (D.3.2.1.2.4) Begin timer for EAR timeout.
                                _timeout_handler->initiate_timeout(this, RANK_EAR_TO_EAR_TIMEOUT);

                                // (D.3.2.1.2.5) Delete UUID from the Store.
                                stop();
                                break;
                            }
                        }
                    }
                } break;
                case MessageType::ACC: {
                    auto acc_message = dynamic_cast<ACC*>(_message);

                    // (E.1) Set pre-reservation as reserved on UUID in Store.
                    _resources->mark_reservation(_reservation);

                    // (E.2) Is UUID in the TranslationTable?
                    if (!is_translation_table_empty_for(_uuid)) {
                        UUIDv4 original_uuid = locate_original_of(_uuid);

                        // (E.2.2.1) Create ACC message to the original UUID.
                        ACC new_acc_message = ACC(original_uuid);
                        // TODO Send this message.

                        // Change state to RESERVED.
                        _state = HandlerState::RESERVED;

                        // (E.2.2.2) Terminate the thread.
                        stop();
                        break;
                    } else {
                        // (E.2.1.1) Create ACC message to the UUID.
                        ACC new_acc_message = ACC(_uuid);
                        // TODO Send this message.

                        // Change state to RESERVED.
                        _state = HandlerState::RESERVED;

                        // (E.2.1.2) Terminate the thread.
                        stop();
                        break;
                    }
                } break;
                case MessageType::REF: {
                    auto ref_message = dynamic_cast<REF*>(_message);

                    // (F.1) Delete pre-reservations with UUID in the Store.
                    // TODO

                    // (F.2) Is UUID in the TranslationTable?
                    if (!is_translation_table_empty_for(_uuid)) {
                        // (F.2.1.1) Delete UUID* in Store.
                        // TODO

                        // (F.2.1.2) Is TranslationTable empty?
                        if (is_translation_table_empty()) {
                            // (F.3) Create REF message and send it.
                            REF new_ref_message = REF(_uuid);
                            // TODO Send message.
                        } else {
                            stop();
                            break;
                        }
                    } else {
                        // (F.3) Create REF message and send it.
                        REF new_ref_message = REF(_uuid);
                        // TODO Send message.
                    }

                    // Change state to CLOSED.
                    _state = HandlerState::CLOSED;

                    // (F.4) Delete UUID from the Store.
                    stop();
                    break;
                } break;
                case MessageType::REP: {
                    auto rep_message = dynamic_cast<REP*>(_message);

                    // Change state to REPLENISHING.
                    _state = HandlerState::REPLENISHING;

                    // (G.1) Check if the current node is the message's listener.
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
                        REF ref_message = REF(_uuid);
                        // TODO Send message.

                        // Change state to CLOSED.
                        _state = HandlerState::CLOSED;

                        // (G.1.1.2) Delete UUID from the Store.
                        stop();
                        break;
                    } else {
                        // (G.1.2.1) Is UUID in Store?
                        if (is_uuid_in_store(_uuid)) {
                            // (G.1.2.1.2.1) Create REP message towards L and send it.
                            REP new_rep_message = REP(_uuid, rep_message->listener_length(), rep_message->listener());

                            // (G.1.2.1.2.2) Begin timer for REP timeout.
                            _timeout_handler->initiate_timeout(this, RANK_REP_TO_REP_TIMEOUT);

                            // (G.1.2.1.2.3) Is UUID in the TranslationTable?
                            UUIDv4 original_uuid = _uuid;
                            if (!is_translation_table_empty_for(_uuid)) {
                                original_uuid = locate_original_of(_uuid);

                                // (G.1.2.1.2.3.1.1) Delete UUID* in Store.
                                // TODO
                            }

                            // (G.1.2.1.2.4) Create REF message with original UUID and send it.
                            REF new_ref_message = REF(original_uuid);
                            // TODO Send message.

                            // Change state to CLOSED.
                            _state = HandlerState::CLOSED;

                            // (G.1.2.1.2.5) Delete UUID from the Store.
                            stop();
                            break;
                        } else {
                            // (G.1.2.1.1.1) Discard and ignore message
                            stop();
                            break;
                        }
                    }
                } break;
            }

            // Delete the pointer of the message.
            delete _message;
            _message = nullptr;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }

    // Closing data before thread death.
}
