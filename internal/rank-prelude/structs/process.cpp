#include "structs/process.h"

Process* Process::get_instance() {
    static auto* instance = new Process();
    return instance;
}

bool Process::store(Handler* handler) {
    // If the store already contains this handler UUID, then return false.
    if (is_uuid_in_store(handler->id())) {
        return false;
    }

    // Save the handler in the store.
    {
        std::lock_guard<std::mutex> lock(_store_locker);
        _store[handler->id()] = handler;
    }

    // Return true as it was seen for the first time now.
    return true;
}

void Process::delete_handler(const UUIDv4& id) {
    auto* handler = get_handler(id);
    if (handler == nullptr) {
        throw std::exception();  // TODO
    }

    // Delete the handler as the pointer and the item in the store.
    delete handler;
    std::lock_guard<std::mutex> lock(_store_locker);
    _store.erase(id);
}

Handler* Process::create_handler(const UUIDv4& id) {
    // Create the handler instance with the given UUID.
    auto* handler = new Handler(_resources, &_translation_table, &_translation_table_locker, _timeout_handler, &_store,
                                &_store_locker, id);

    // Associate the handler item in the store.
    {
        std::lock_guard<std::mutex> lock(_store_locker);
        _store[id] = handler;
    }

    // Return reference from the handler within the context of the store.
    return handler;
}

Handler* Process::resume_handler(const UUIDv4& id) {
    // Get handler from the store.
    auto* handler = get_handler(id);
    if (handler == nullptr) {
        throw std::exception();  // TODO
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
        throw std::exception();  // TODO
    }

    // Stop the handler's thread.
    handler->stop();

    // Return the handler pointer.
    return handler;
}

Handler* Process::get_handler(const UUIDv4& id) {
    // If the store has the UUID, then return its handler; otherwise nullptr.
    if (is_uuid_in_store(id)) {
        std::lock_guard<std::mutex> lock(_store_locker);
        return _store.at(id);
    } else {
        return nullptr;
    }
}

HandlerState Process::get_handler_state(const UUIDv4& id) {
    // Get the handler with the given UUID.
    auto* handler = get_handler(id);
    if (handler != nullptr) {
        return handler->state();
    }

    // If the handler does not exist, throw exception.
    throw std::exception();  // TODO
}

bool Process::is_uuid_in_store(const UUIDv4& id) {
    std::lock_guard<std::mutex> lock(_store_locker);
    return _store.contains(id);
}

Header Process::parse_as_message_header(const uint8_t* data) {
    return new_prototype(data);
}

MessageType Process::parse_as_message_type(const uint8_t* data) {
    return parse_as_message_header(data).type();
}

UUIDv4 Process::parse_as_message_uuid(const uint8_t* data) {
    return parse_as_message_header(data).uuid();
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
    while (_running) {
        // Prepare for a new connection.
        int new_connection = accept(_socket, (struct sockaddr*) &_address, &_address_length);
        // If a new connection is wrongly done, simply ignore it.
        if (new_connection < 0) {
            continue;
        }

        // Read the data from the new connection's socket.
        uint8_t raw_data[8192] = {0};
        ssize_t raw_data_size = read(new_connection, raw_data, 8192 - 1);

        // Get the UUID from the raw data received.
        auto message_uuid = parse_as_message_uuid(raw_data);

        // Create a meta handler.
        Handler* handler = nullptr;

        // If UUID is known (A.3)...
        if (is_uuid_in_store(message_uuid)) {
            // Check the state of such UUID.
            auto uuid_state = _store[message_uuid]->state();

            // If state is CLOSED, then simply close the socket and ignore.
            if (uuid_state == HandlerState::CLOSED) {
                close(new_connection);
                continue;
            } else {
                // Otherwise, resume handling the request.
                handler = resume_handler(message_uuid);
            }
        } else {
            // If the UUID is not known in the Store, then create one and save it.
            handler = create_handler(message_uuid);
            _store[message_uuid] = handler;
            handler->execute();
        }

        // Parse the raw data as a message header and pass a complete message to the handler to handle.
        Header message_header = parse_as_message_header(raw_data);
        std::vector<uint8_t> message_body(raw_data_size - RANK_HEADER_LEN);
        std::memcpy(message_body.data(), raw_data+RANK_HEADER_LEN, raw_data_size-RANK_HEADER_LEN);

        switch (message_header.type()) {
            case MessageType::EAR:
                handler->handle(new EAR(message_header, message_body));
                break;
            case MessageType::MAR:
                handler->handle(new MAR(message_header, message_body));
                break;
            case MessageType::BID:
                handler->handle(new BID(message_header, message_body));
                break;
            case MessageType::ACC:
                handler->handle(new ACC(message_header));
                break;
            case MessageType::REF:
                handler->handle(new REF(message_header));
                break;
            case MessageType::REP:
                handler->handle(new REP(message_header, message_body));
                break;
            case MessageType::NOTYPE:
                // TODO Handle this case.
                break;
        }

        // Close the new connection socket as data is already retrieved.
        close(new_connection);

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }

    // Close the base socket.
    close(_socket);
}

Process::~Process() {
    // Stop running the base thread.
    stop();
}

Process::Process() {
    // Initialize the inner structures.
    _resources = Resources::get_instance();
    _timeout_handler = TimeoutHandler::get_instance();

    // Initialize the socket.
    _socket = socket(AF_INET, SOCK_STREAM, 0);  // TODO Handle also when AF_LOCAL or AF_INET6.
    if (_socket < 0) {
        throw std::exception();  // TODO
    }
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_socket_options, sizeof(_socket_options))) {
        throw std::exception();  // TODO
    }
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(RANK_PORT);
    if (bind(_socket, (struct sockaddr*) &_address, sizeof(_address)) < 0) {
        throw std::exception();  // TODO
    }
    if (listen(_socket, 3) < 0) {
        throw std::exception();  // TODO
    }
}
