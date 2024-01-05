#include "structs/process.h"

Process* Process::get_instance() {
    static auto* instance = new Process();
    return instance;
}

bool Process::is_bid_in_store(const UUIDv4& id) const {
    // If the store does not have the UUID, return false; otherwise check if pre-reserved.
    if (!is_uuid_in_store(id)) {
        return false;
    } else {
        return _store.at(id)->state() == HandlerState::PRE_RESERVED;
    }
}

bool Process::is_uuid_in_store(const UUIDv4& id) const {
    return _store.contains(id);
}

bool Process::store(Handler* handler) {
    // If the store already contains this handler UUID, then return false.
    if (is_uuid_in_store(handler->id())) {
        return false;
    }

    // Save the handler in the store.
    _store[handler->id()] = handler;

    // Return true as it was seen for the first time now.
    return true;
}

void Process::delete_handler(const UUIDv4& id) {
    auto* handler = get_handler(id);
    if (handler == nullptr) {
        throw std::exception(); // TODO
    }

    // Delete the handler as the pointer and the item in the store.
    delete handler;
    _store.erase(id);
}

Handler* Process::create_handler(const UUIDv4& id) {
    // Create the handler instance with the given UUID.
    auto* handler = new Handler(id);

    // Associate the handler item in the store.
    _store[id] = handler;

    // Return reference from the handler within the context of the store.
    return handler;
}

Handler* Process::resume_handler(const UUIDv4& id) {
    // Get handler from the store.
    auto* handler = get_handler(id);
    if (handler == nullptr) {
        throw std::exception(); // TODO
    }

    // Execute the handler's thread.
    handler->execute();

    // Return the handler pointer.
    return handler;
}

Handler* Process::suspend_handler(const UUIDv4& id) {
    // Get handler from the store.
    auto* handler = get_handler(id);
    if (handler == nullptr) {
        throw std::exception(); // TODO
    }

    // Stop the handler's thread.
    handler->stop();

    // Return the handler pointer.
    return handler;
}

Handler* Process::get_handler(const UUIDv4& id) const {
    // If the store has the UUID, then return its handler; otherwise nullptr.
    if (is_uuid_in_store(id)) {
        return _store.at(id);
    } else {
        return nullptr;
    }
}

HandlerState Process::get_handler_state(const UUIDv4& id) const {
    // Get the handler with the given UUID.
    auto* handler = get_handler(id);
    if (handler != nullptr) {
        return handler->state();
    }

    // If the handler does not exist, throw exception.
    throw std::exception(); // TODO
}

Header Process::parse_as_message_header(const char* data) {
    return Message::new_prototype(data);
}

MessageType Process::parse_as_message_type(const char* data) {
    return parse_as_message_header(data).type();
}

UUIDv4 Process::parse_as_message_uuid(const char* data) {
    return parse_as_message_header(data).uuid();
}

std::vector<std::string> Process::get_connections_to(const std::string& target) const {}

unsigned int Process::connections_cardinal(const std::vector<std::string>& connections) const {
    return connections.size();
}

Process* Process::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

Process* Process::stop() {
    if (!_running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool Process::is_running() const {
    return _running;
}

void Process::operator()() {
    // Initialization for the threading mechanism.

    while (_running) {
        // Tasks to be performed periodically within the thread.

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }

    // Closing data before thread death.

}

Process::Process() {
    // Initialization process.

}
