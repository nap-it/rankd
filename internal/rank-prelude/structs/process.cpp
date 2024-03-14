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

Header Process::parse_as_message_header(const std::vector<uint8_t>& data) {
    assert(data.size() >= RANK_HEADER_LEN);

    std::array<uint8_t, RANK_HEADER_LEN> data_bytes{};
    std::copy_n(data.begin(), RANK_HEADER_LEN, data_bytes.begin());

    return new_prototype(data_bytes);
}

MessageType Process::parse_as_message_type(const std::vector<uint8_t>& data) {
    return parse_as_message_header(data).type();
}

UUIDv4 Process::parse_as_message_uuid(const std::vector<uint8_t>& data) {
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
        // Check depositing queue of the dispatcher and retrieve a message, if there is one.
        if (_dispatcher->receiving_queue_has_message()) {
            // Dequeue a message from the Dispatcher's depositing queue.
            Message* message;
            std::vector<uint8_t> source_address;
            IdentifierType source_address_type;
            std::tie(message, source_address, source_address_type) = _dispatcher->dequeue_item();

            // Get the UUID from the raw data received.
            auto message_uuid = message->uuid();

            // Create a meta handler.
            Handler *handler = nullptr;

            // If UUID is known (A.3)...
            if (is_uuid_in_store(message_uuid)) {
                // Check the state of such UUID.
                auto uuid_state = _store[message_uuid]->state();

                // If state is CLOSED, then simply close the socket and ignore.
                if (uuid_state == HandlerState::CLOSED) {
                    continue;
                } else {
                    // Otherwise, resume handling the request.
                    handler = resume_handler(message_uuid);
                }
            } else {
                // If the UUID is not known in the Store, then create one and save it.
                handler = create_handler(message_uuid);
                _store[message_uuid] = handler;
                handler->borrow(_dispatcher)->execute();
            }

            // Parse the raw data as a message header and pass a complete message to the handler to handle.
            Header message_header = message->header();

            switch (message_header.type()) {
                case MessageType::EAR:
                    // Get the source of this message and set it on the handler.
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    handler->handle(dynamic_cast<EAR*>(message));
                    break;
                case MessageType::MAR:
                    // Get the source of this message and set it on the handler.
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    handler->handle(dynamic_cast<MAR*>(message));
                    break;
                case MessageType::BID:
                    // Get the source of this message and set it on the handler.
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    handler->handle(dynamic_cast<BID*>(message));
                    break;
                case MessageType::ACC:
                    // Get the source of this message and set it as an accepting node in the handler.
                    handler->mark_accepting_node(std::make_pair(source_address, source_address_type));

                    handler->handle(dynamic_cast<ACC*>(message));
                    break;
                case MessageType::REF:
                    handler->handle(dynamic_cast<REF*>(message));
                    break;
                case MessageType::REP:
                    // Get the source of this message and set it on the handler.
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    handler->handle(dynamic_cast<REP*>(message));
                    break;
                case MessageType::NOTYPE:
                    // TODO Handle this case.
                    break;
            }
        }

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

    // Initialize the rank0 network interface.
    struct sockaddr_nl binding_address{};
    int netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (netlink_socket < 0) {
        throw std::ios_base::failure("Netlink could not be opened in order to create a rank0 interface in Process.");
    }
    memset(&binding_address, 0, sizeof(binding_address));
    binding_address.nl_family = AF_NETLINK;
    binding_address.nl_pid = 0;
    if (bind(netlink_socket, (struct sockaddr*) &binding_address, sizeof(binding_address)) < 0) {
        throw std::ios_base::failure("Netlink socket could not be bound in rankd Process.");
    }
    struct {
        struct nlmsghdr header;
        struct ifinfomsg message;
    } request;
    request.header.nlmsg_type = RTM_NEWLINK;
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_EXCL | NLM_F_CREATE;
    request.header.nlmsg_len = sizeof(request);
    request.header.nlmsg_seq = time(nullptr);
    request.message.ifi_family = AF_UNSPEC;
    request.message.ifi_type = ARPHRD_NETROM;
    request.message.ifi_index = 0;
    request.message.ifi_flags = IFF_UP;
    request.message.ifi_change = 0xFFFFFFFF;
    struct rtattr* rta = (struct rtattr*) (((char*) &request) + NLMSG_ALIGN(request.header.nlmsg_len));
    rta->rta_type = IFLA_IFNAME;
    rta->rta_len = RTA_LENGTH(sizeof(RANK_INTERFACE)+1);
    request.header.nlmsg_len = NLMSG_ALIGN(request.header.nlmsg_len) + RTA_LENGTH(sizeof(RANK_INTERFACE)+1);
    memcpy(RTA_DATA(rta), RANK_INTERFACE, sizeof(RANK_INTERFACE)+1);
    rta = (struct rtattr*) (((char*) &request) + NLMSG_ALIGN(request.header.nlmsg_len));
    rta->rta_type = IFLA_LINKINFO;
    rta->rta_len = RTA_LENGTH(sizeof(short));
    struct rtattr info_kind_attribute;
    info_kind_attribute.rta_type = IFLA_INFO_KIND;
    info_kind_attribute.rta_len = RTA_LENGTH(sizeof(RANK_INTERFACE_TYPE_INFO)+1);
    request.header.nlmsg_len = NLMSG_ALIGN(request.header.nlmsg_len) + RTA_LENGTH(sizeof(info_kind_attribute));
    memcpy(RTA_DATA(rta), &info_kind_attribute, sizeof(info_kind_attribute));

    ssize_t sent_bytes = send(netlink_socket, &request, request.header.nlmsg_len, 0);
    if (sent_bytes < 0) {
        close(netlink_socket);
        throw std::ios_base::failure("Netlink socket could not send data in rankd Process to configure rank0 interface.");
    }
    char response_buffer[8192];
    ssize_t received_bytes = recv(netlink_socket, response_buffer, sizeof(response_buffer), 0);
    if (received_bytes < 0) {
        close(netlink_socket);
        throw std::ios_base::failure("Rankd Process failed to receive a response from the Netlink socket on creating the rank0 interface.");
    }
    auto* response = (struct nlmsghdr*) response_buffer;
    while (NLMSG_OK(response, received_bytes)) {
        if (response->nlmsg_type == NLMSG_ERROR) {
            if (((struct nlmsgerr*) response)->error != 0) {
                close(netlink_socket);
                throw std::ios_base::failure("The interface rank0 could not be successfully set.");
            }
        }
    }
    close(netlink_socket);

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

    // Initialize the dispatcher.
    _dispatcher = Dispatcher::get_instance();
}
