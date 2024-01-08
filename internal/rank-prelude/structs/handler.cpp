#include "structs/handler.h"

Handler::Handler(Resources* resources, TranslationTable* translation_table) {}

Handler::Handler(Resources* resources, TranslationTable* translation_table, UUIDv4 uuid) {}

Handler::Handler(Resources* resources, TranslationTable* translation_table, Header header) {
    // Update all attributes from caller.
    _uuid = header.uuid();
    _bids = {};

    // Update inner structures from caller.
    _resources = resources;
    _translation_table = translation_table;

    // Initialize other attributes.
    _reservation = nullptr;
    _timeout_spec = nullptr;

    // Change handler's state to Ready.
    _state = HandlerState::READY;
}

Handler* Handler::handle(const Message& message) {}

bool Handler::create_translation(const UUIDv4& to, const UUIDv4& from) const {}

const UUIDv4& Handler::locate_original_of(const UUIDv4& translated) const {}

bool Handler::is_translation_table_empty_for(const UUIDv4& uuid) const {}

void Handler::new_bid(float bid, std::array<uint8_t, 4>& ipv4_address) {}

void Handler::new_bid(float bid, std::array<uint8_t, 6>& mac_address) {}

void Handler::new_bid(float bid, std::array<uint8_t, 16>& ipv6_address) {}

void Handler::clear_bids() {
    _bids.clear();
}

size_t Handler::cardinal_bids() {
    return _bids.size();
}

std::set<std::string> Handler::min_bids() const {}

bool Handler::is_min_bid_unique(const std::set<std::string>& targets) const {}

Reservation Handler::produce_reservation(const std::vector<RequestingCapabilities>& capabilities) const {}

UUIDv4 Handler::id() const {
    return _uuid;
}

HandlerState Handler::state() const {
    return _state;
}

Reservation* Handler::associated_reservation() const {
    return _reservation;
}

bool Handler::is_me(std::array<uint8_t, 4>& ipv4_address) {}

bool Handler::is_me(std::array<uint8_t, 6>& mac_address) {}

bool Handler::is_me(std::array<uint8_t, 16>& ipv6_address) {}

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
        // Tasks to be performed periodically within the thread.

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }

    // Closing data before thread death.

}
