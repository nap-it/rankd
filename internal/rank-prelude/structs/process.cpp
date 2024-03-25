#include "structs/process.h"

using namespace rank;

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
    _logger->trace("[Process] Executing a Rank Process...");
    if (_running) {
        _logger->warn("[Process] A process was already running. Someone is calling for execution again.");
        return this;
    }

    _logger->trace("[Process] Starting the main thread...");
    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

Process* Process::stop() {
    _logger->trace("[Process] Stopping a Rank Process...");
    if (!_running) {
        _logger->warn("[Process] The process was already stopped. Someone is calling to stop again.");
        return this;
    }

    _logger->trace("[Process] Stopping the main thread...");
    _running = false;
    _thread.join();

    _logger->info("The Process has been stopped.");

    return this;
}

bool Process::is_running() const {
    return _running;
}

void Process::operator()() {
    _logger->info("Starting the process main thread...");
    _logger->info("Process is now waiting for messages in the dispatcher's receiving queue at each {} seconds.", _waiting_time);

    while (_running) {
        // Check depositing queue of the dispatcher and retrieve a message, if there is one.
        if (_dispatcher->receiving_queue_has_message()) {
            _logger->info("The process watched a message being dropped in the dispatcher's receiving queue.");

            // Dequeue a message from the Dispatcher's depositing queue.
            Message* message;
            std::vector<uint8_t> source_address;
            IdentifierType source_address_type;
            std::tie(message, source_address, source_address_type) = _dispatcher->dequeue_item();

            // Get the UUID from the raw data received.
            auto message_uuid = message->uuid();

            _logger->debug("[Process] Outside --> Rank: Message UUID of {}.", message_uuid);
            _logger->debug("[Process] Outside --> Rank: Message source address type of {}.", identifier_to_string(source_address_type));

            // Create a meta handler.
            Handler *handler = nullptr;

            // If UUID is known (A.3)...
            _logger->trace("[Process] [{}] Checking if the UUID is already known...", message_uuid);
            if (is_uuid_in_store(message_uuid)) {
                // Check the state of such UUID.
                auto uuid_state = _store[message_uuid]->state();

                _logger->trace("[Process] [{}] This UUID is in store and its handler is in {} state.", message_uuid, handler_state_to_string(uuid_state));

                // If state is CLOSED, then simply close the socket and ignore.
                if (uuid_state == HandlerState::CLOSED) {
                    _logger->info("Ignoring message since its UUID {} reports being closed.", message_uuid);
                    continue;
                } else {
                    // Otherwise, resume handling the request.
                    _logger->trace("[Process] [{}] Resuming handler on this UUID.", message_uuid);
                    handler = resume_handler(message_uuid);
                }
            } else {
                // If the UUID is not known in the Store, then create one and save it.
                _logger->trace("[Process] [{}] This UUID does not exist yet. Registering...", message_uuid);
                handler = create_handler(message_uuid);
                _store[message_uuid] = handler;

                _logger->trace("[Process] [{}] The handler was created and will be executed now...", message_uuid);
                handler->borrow(_dispatcher)->execute();
            }

            _logger->info("A handler for {} is ready to receive message.", message_uuid);

            // Parse the raw data as a message header and pass a complete message to the handler to handle.
            Header message_header = message->header();

            _logger->trace("[Process] [{}] Getting the type of message received in this instance.", message_uuid);
            switch (message_header.type()) {
                case MessageType::EAR:
                    _logger->trace("[Process] [{}] This message is an EAR message.", message_uuid);

                    // Get the source of this message and set it on the handler.
                    _logger->trace("[Process] [{}] Marking the handler's source address.", message_uuid);
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", message_uuid);
                    handler->handle(dynamic_cast<EAR*>(message));
                    break;
                case MessageType::MAR:
                    _logger->trace("[Process] [{}] This message is a MAR message.", message_uuid);

                    // Get the source of this message and set it on the handler.
                    _logger->trace("[Process] [{}] Marking the handler's source address.", message_uuid);
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", message_uuid);
                    handler->handle(dynamic_cast<MAR*>(message));
                    break;
                case MessageType::BID:
                    _logger->trace("[Process] [{}] This message is a BID message.", message_uuid);

                    // Get the source of this message and set it on the handler.
                    _logger->trace("[Process] [{}] Marking the handler's source address.", message_uuid);
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", message_uuid);
                    handler->handle(dynamic_cast<BID*>(message));
                    break;
                case MessageType::ACC:
                    _logger->trace("[Process] [{}] This message is an ACC message.", message_uuid);

                    // Get the source of this message and set it as an accepting node in the handler.
                    _logger->trace("[Process] [{}] Marking the handler's accepting node address.", message_uuid);
                    handler->mark_accepting_node(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", message_uuid);
                    handler->handle(dynamic_cast<ACC*>(message));
                    break;
                case MessageType::REF:
                    _logger->trace("[Process] [{}] This message is a REF message.", message_uuid);

                    _logger->trace("[Process] [{}] Passing message to the handler.", message_uuid);
                    handler->handle(dynamic_cast<REF*>(message));
                    break;
                case MessageType::REP:
                    _logger->trace("[Process] [{}] This message is a REP message.", message_uuid);

                    // Get the source of this message and set it on the handler.
                    _logger->trace("[Process] [{}] Marking the handler's source address.", message_uuid);
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", message_uuid);
                    handler->handle(dynamic_cast<REP*>(message));
                    break;
                case MessageType::NOTYPE:
                    _logger->error("[Process] [{}] This message type is supposed to not exist... Nothing to do here...", message_uuid);
                    // TODO Handle this case.
                    break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }
}

#ifndef SIMUZILLA
Process *Process::borrow_simulation_recv_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>(void)> *function) {
    _logger->trace("[Process] Registering Rx function in dispatcher.");
    _dispatcher->borrow_simulation_receiver_function(function);

    _in_simulation = true;

    return this;
}

Process *
Process::borrow_simulation_send_function(std::function<void(uint8_t, const std::vector<uint8_t> &)> *function) {
    _logger->trace("[Process] Registering Tx function in dispatcher.");
    _dispatcher->borrow_simulation_sender_function(function);

    _in_simulation = true;

    return this;
}
#endif

Process *Process::log_on(std::shared_ptr<spdlog::logger> logger) {
    _logger = logger;

    // TODO Change all the other entities logger's too.

    return this;
}

Process::~Process() {
    _logger->info("The Process is being destroyed.");
    // Stop running the base thread.
    stop();
}

Process::Process() {
    _logger->info("Preparing the Rank process...");

    // Initialize the inner structures.
    _logger->trace("[Process] Retrieving resources and the timeout handler. (Step 1 of 6)");
    _resources = Resources::get_instance();
    _timeout_handler = TimeoutHandler::get_instance();

    // Initialize the rank0 network interface.
    _logger->trace("[Process] Creating rank0 network interface. (Step 2 of 6)");
    struct sockaddr_nl binding_address{};
    int netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (netlink_socket < 0) {
        _logger->critical("[Process] Netlink could not be opened in order to create a rank0 interface in Process.");
        throw std::ios_base::failure("");
    }
    memset(&binding_address, 0, sizeof(binding_address));
    binding_address.nl_family = AF_NETLINK;
    binding_address.nl_pid = 0;
    if (bind(netlink_socket, (struct sockaddr*) &binding_address, sizeof(binding_address)) < 0) {
        _logger->critical("[Process] Netlink socket could not be bound in rankd Process.");
        throw std::ios_base::failure("");
    }
    _logger->trace("[Process] Creating netlink message to kernel for network interface creation. (Step 3 of 6)");
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

    _logger->trace("[Process] Sending message to kernel for network interface creation. (Step 4 of 6)");
    ssize_t sent_bytes = send(netlink_socket, &request, request.header.nlmsg_len, 0);
    _logger->debug("[Process] Rank --> Kernel: {} bytes were sent.", sent_bytes);
    if (sent_bytes < 0) {
        _logger->critical("[Process] Netlink socket could not send data in rankd Process to configure rank0 interface.");
        close(netlink_socket);
        throw std::ios_base::failure("");
    }

    _logger->trace("[Process] Waiting for the kernel answer... (Step 5 of 6)");
    char response_buffer[8192];
    ssize_t received_bytes = recv(netlink_socket, response_buffer, sizeof(response_buffer), 0);
    _logger->debug("[Process] Rank <-- Kernel: {} bytes were received.", received_bytes);
    if (received_bytes < 0) {
        _logger->critical("[Process] Rank Process failed to receive a response from the Netlink socket on creating the rank0 interface.");
        close(netlink_socket);
        throw std::ios_base::failure("");
    }

    _logger->trace("[Process] Parsing the received bytes as a kernel's netlink message. (Step 6 of 6)");
    auto* response = (struct nlmsghdr*) response_buffer;
    while (NLMSG_OK(response, received_bytes)) {
        if (response->nlmsg_type == NLMSG_ERROR) {
            if (((struct nlmsgerr*) response)->error != 0) {
                _logger->critical("[Process] The interface rank0 could not be successfully set.");
                close(netlink_socket);
                throw std::ios_base::failure("");
            }
        }
    }
    close(netlink_socket);
    _logger->info("The Process is prepared for running.");

    // Initialize the dispatcher.
    _logger->info("Initializing the dispatcher...");
    _dispatcher = Dispatcher::get_instance();
}
