#include "structs/process.h"

using namespace rank;

Process* Process::get_instance(const std::string& logger_name) {
#ifdef FROM_SIMUZILLA
    return new Process(logger_name);
#else
    static auto* instance = new Process(logger_name);
    return instance;
#endif
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
                                &_store_locker, id, _logger->name());

    // Register functions for API communication.
    handler->register_api_methods([this](const UUIDv4& uuid) -> bool { return am_i_origin_for(uuid); }, [this](const UUIDv4& uuid) -> bool { return remove_as_origin(uuid); });

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

bool Process::am_i_origin_for(const UUIDv4 &uuid) const {
    return _origin_set.contains(uuid);
}

void Process::mark_origin(const UUIDv4 &uuid, uint32_t pid) {
    std::lock_guard<std::mutex> guard(_origin_set_locker);
    _origin_set.insert({uuid, pid});
}

bool Process::remove_as_origin(const UUIDv4 &uuid) {
    std::lock_guard<std::mutex> guard(_origin_set_locker);
    if (not am_i_origin_for(uuid)) {
        return false;
    }

    _origin_set.erase(uuid);

    return true;
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
    _dispatcher->execute_dispatchers();
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
    _dispatcher->stop_dispatchers();
    _thread.join();

    _logger->info("The Process has been stopped.");

    return this;
}

bool Process::is_running() const {
    return _running;
}

void Process::operator()() {
#ifdef FROM_SIMUZILLA
    // Initialize the inner structures.
    _resources = Resources::get_instance(_simulated_topology, _simuzilla_identity, _logger->name());
    _resources->execute();
    _timeout_handler = TimeoutHandler::get_instance(_logger->name());
#endif
    _logger->info("Starting the process main thread...");
    _logger->info("Process is now waiting for messages in the dispatcher's receiving queue at each {} seconds.", _waiting_time);

    while (_running) {
        _logger->debug("[Process] (A.2) Wait for Packet.");
        // Check depositing queue of the dispatcher and retrieve a message, if there is one. (A.2)
        if (_dispatcher->receiving_queue_has_message()) {
            _logger->info("The process watched a message being dropped in the dispatcher's receiving queue.");

            // Dequeue a message from the Dispatcher's depositing queue.
            Message* message;
            std::vector<uint8_t> source_address;
            IdentifierType source_address_type;
            std::tie(message, source_address, source_address_type) = _dispatcher->dequeue_item();

            // Get the UUID from the raw data received.
            auto message_uuid = message->uuid();

            _logger->trace("[Process] Outside --> Rank: Message UUID of {}.", display(message_uuid));
            _logger->trace("[Process] Outside --> Rank: Message source address type of {}.", identifier_to_string(source_address_type));

            // Create a meta handler.
            Handler *handler = nullptr;

            // If UUID is known (A.3)...
            if (is_uuid_in_store(message_uuid)) {
                _logger->debug("[Process] (A.3) Is UUID {} already known? Yes.", display(message_uuid));
                // Check the state of such UUID.
                auto uuid_state = _store[message_uuid]->state();

                _logger->trace("[Process] The UUID {} is in store and its handler is in {} state.", display(message_uuid), handler_state_to_string(uuid_state));

                // (A.3.1.1) Is UUID's handler running?
                if (get_handler(message_uuid)->is_running()) {
                    _logger->debug("[Process] (A.3.1.1) Is UUID's handler running? Yes.");
                    auto* handler = get_handler(message_uuid);

                    // (A.3.1.1.1.1) Is UUID's handler in AUCTION_WAITING?
                    if (handler->state() == HandlerState::AUCTION_WAITING) {
                        _logger->debug("[Process] (A.3.1.1.1.1) Is UUID's handler in AUCTION_WAITING? Yes.");

                        // (A.3.1.1.1.1.1.1) Is message type BID?
                        if (message->type() == MessageType::BID) {
                            _logger->debug("[Process] (A.3.1.1.1.1.1.1) Is message type BID? Yes.");

                            auto bid_message = dynamic_cast<BID*>(message);

                            // (A.3.1.1.1.1.1.2) Leave bid value in Store bid set.
                            _logger->debug("[Process] (A.3.1.1.1.1.1.2) Leave bid value of in Store bid set.");
                            _logger->trace("[Process] Bid value of {}.", bid_message->value());

                            switch (source_address_type) {
                                case IdentifierType::Simulation:
                                    handler->new_bid(bid_message->value(), source_address.at(0));
                                    break;
                                case IdentifierType::MAC: {
                                    std::array<uint8_t, MAC_ADDR_LEN> address{};
                                    std::copy_n(source_address.begin(), MAC_ADDR_LEN, address.begin());
                                    handler->new_bid(bid_message->value(), address);
                                }
                                    break;
                                case IdentifierType::IPv4: {
                                    std::array<uint8_t, IPV4_ADDR_LEN> address{};
                                    std::copy_n(source_address.begin(), IPV4_ADDR_LEN, address.begin());
                                    handler->new_bid(bid_message->value(), address);
                                }
                                    break;
                                case IdentifierType::IPv6: {
                                    std::array<uint8_t, IPV6_ADDR_LEN> address{};
                                    std::copy_n(source_address.begin(), IPV6_ADDR_LEN, address.begin());
                                    handler->new_bid(bid_message->value(), address);
                                }
                                    break;
                                case IdentifierType::DDS: {
                                    std::string address;
                                    std::copy_n(source_address.begin(), DDS_ADDR_LEN, address.begin());
                                    handler->new_bid(bid_message->value(), address);
                                }
                                    break;
                            }

                            // (A.3.1.1.2.1.1.1) Discard message
                            _logger->info("Discarding message for UUID {}.", display(message_uuid));
                            continue;
                        } else {
                            _logger->debug("[Process] (A.3.1.1.1.1.1.1) Is message type BID? No.");

                            // (A.3.1.1.2.1.1.1) Discard message
                            _logger->info("Discarding message for UUID {}.", display(message_uuid));
                            continue;
                        }
                    } else {
                        _logger->debug("[Process] (A.3.1.1.1.1) Is UUID's handler in AUCTION_WAITING? No.");

                        // (A.3.1.1.2.1.1.1) Discard message
                        _logger->info("Discarding message for UUID {}.", display(message_uuid));
                        continue;
                    }
                } else {
                    _logger->debug("[Process] (A.3.1.1) Is UUID's handler running? No");

                    switch (uuid_state) {
                        // If state is ASSESSING, PRESENTING, AUCTION_BIDDING, REPLENISHING, or CLOSED,
                        // then simply close the socket and ignore. (A.3.1.1.1.1)
                        case HandlerState::ASSESSING:
                        case HandlerState::PRESENTING:
                        case HandlerState::AUCTION_BIDDING:
                        case HandlerState::REPLENISHING:
                        case HandlerState::CLOSED:
                            _logger->debug("[Process] (A.3.1.1.2.1) What is the found handler state? {}",
                                           handler_state_to_string(uuid_state));

                            // (A.3.1.1.2.1.1.1) Discard message
                            _logger->info("Discarding message for UUID {}.", display(message_uuid));
                            continue;
                        case HandlerState::PRE_RESERVED:
                        case HandlerState::AUCTION_WAITING:
                        case HandlerState::RESERVED:
                        case HandlerState::READY:
                            _logger->debug("[Process] (A.3.1.1.2.1) What is the found handler state? {}",
                                           handler_state_to_string(uuid_state));

                            // (A.3.1.1.2.1.2.1) Get handler h from Store.
                            _logger->debug("[Process] (A.3.1.1.2.1.2.1) Get handler h from Store.");
                            _logger->debug("[Process] (A.3.1.1.2.1.2.2) Execute lifecycle of handler h.");

                            _logger->trace("[Process] Resuming handler on UUID {}.", display(message_uuid));
                            handler = resume_handler(message_uuid);
                    }
                }
            } else {
                _logger->debug("[Process] (A.3) Is UUID {} already known? No.", display(message_uuid));

                _logger->debug("[Process] (A.3.2.1) Create handler h with this new UUID.");
                handler = create_handler(message_uuid);

                _logger->debug("[Process] (A.3.2.2) Save h in Store.");
                _store[message_uuid] = handler;

                _logger->debug("[Process] (A.3.2.3) Execute lifecycle of handler h.");
                handler->borrow(_dispatcher)->execute();
#ifdef FROM_SIMUZILLA
                handler->borrow(_simulated_connections);
                handler->borrow(_simulated_identity);
#endif
            }

            _logger->info("A handler for {} is ready to receive message.", display(message_uuid));

            // Parse the raw data as a message header and pass a complete message to the handler to handle.
            Header message_header = message->header();

            _logger->trace("[Process] [{}] Getting the type of message received in this instance.", display(message_uuid));

            // Parse the message type. (A.4)
            switch (message_header.type()) {
                case MessageType::EAR:
                    _logger->debug("[Process] (A.4) What is this message type? EAR.");

                    // Get the source of this message and set it on the handler.
                    _logger->trace("[Process] [{}] Marking the handler's source address.", display(message_uuid));
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", display(message_uuid));
                    handler->handle(dynamic_cast<EAR*>(message));
                    break;
                case MessageType::MAR:
                    _logger->debug("[Process] (A.4) What is this message type? MAR.");

                    // Get the source of this message and set it on the handler.
                    _logger->trace("[Process] [{}] Marking the handler's source address.", display(message_uuid));
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", display(message_uuid));
                    handler->handle(dynamic_cast<MAR*>(message));
                    break;
                case MessageType::BID:
                    _logger->debug("[Process] (A.4) What is this message type? BID.");

                    // Get the source of this message and set it on the handler.
                    _logger->trace("[Process] [{}] Marking the handler's source address.", display(message_uuid));
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", display(message_uuid));
                    handler->handle(dynamic_cast<BID*>(message));
                    break;
                case MessageType::ACC:
                    _logger->debug("[Process] (A.4) What is this message type? ACC.");

                    // Get the source of this message and set it as an accepting node in the handler.
                    _logger->trace("[Process] [{}] Marking the handler's accepting node address.", display(message_uuid));
                    handler->mark_accepting_node(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", display(message_uuid));
                    handler->handle(dynamic_cast<ACC*>(message));
                    break;
                case MessageType::REF:
                    _logger->debug("[Process] (A.4) What is this message type? REF.");

                    _logger->trace("[Process] [{}] Passing message to the handler.", display(message_uuid));
                    handler->handle(dynamic_cast<REF*>(message));
                    break;
                case MessageType::REP:
                    _logger->debug("[Process] (A.4) What is this message type? REP.");

                    // Get the source of this message and set it on the handler.
                    _logger->trace("[Process] [{}] Marking the handler's source address.", display(message_uuid));
                    handler->mark_source(std::make_pair(source_address, source_address_type));

                    _logger->trace("[Process] [{}] Passing message to the handler.", display(message_uuid));
                    handler->handle(dynamic_cast<REP*>(message));
                    break;
                case MessageType::NOTYPE:
                    _logger->error("[Process] [{}] This message type is supposed to not exist... Nothing to do here...", display(message_uuid));
                    // TODO Handle this case.
                    break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(_waiting_time));
    }
}

#ifdef FROM_SIMUZILLA
Process* Process::set_topology_and_current_address(std::function<const std::vector<int>*()> topology, unsigned int address) {
    _simulated_topology = topology;
    _simuzilla_identity = address;

    _dispatcher->set_topology_and_current_address(topology, address);

    return this;
}

Process *Process::borrow_simulation_recv_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>(void)> function) {
    _logger->trace("[Process] Registering Rx function in dispatcher.");
    _dispatcher->borrow_simulation_receiver_function(std::move(function));

    _in_simulation = true;

    return this;
}

Process *
Process::borrow_simulation_send_function(std::function<void(uint8_t, std::vector<uint8_t>)> function) {
    _logger->trace("[Process] Registering Tx function in dispatcher.");
    _dispatcher->borrow_simulation_sender_function(std::move(function));

    _in_simulation = true;

    return this;
}

Process *Process::borrow_simulation_connections_function(std::function<std::vector<std::pair<uint8_t, uint8_t>>(uint8_t)> function) {
    _logger->trace("[Process] Registering connections function in process.");
    _simulated_connections = std::move(function);

    _in_simulation = true;

    return this;
}

Process *Process::borrow_simulation_identity_function(std::function<bool(uint8_t)> function) {
    _logger->trace("[Process] Registering identity function in process.");
    _simulated_identity = std::move(function);

    _in_simulation = true;

    return this;
}

API *Process::use_api() {
    return _dispatcher->api();
}
#endif

Process *Process::log_on(const std::string& logger_name) {
    _logger = spdlog::get(logger_name);

    // TODO Change all the other entities logger's too.

    return this;
}

Process::~Process() {
    _logger->info("The Process is being destroyed.");
    // Stop running the base thread.
    stop();
}

Process::Process(const std::string& logger_name) {
#ifndef FROM_SIMUZILLA
    // Configure logging.
    _logger = spdlog::get(logger_name);
    _logger->info("Preparing the Rank process...");

    // Initialize the inner structures.
    _logger->trace("[Process] Retrieving resources and the timeout handler. (Step 1 of 6)");
    _resources = Resources::get_instance();
    _resources->execute();
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
    _dispatcher = Dispatcher::get_instance([this](const UUIDv4& uuid, uint32_t pid = 0) -> void { return mark_origin(uuid, pid); }, _logger->name());
#else
    _logger = spdlog::get(logger_name);
    _logger->info("Preparing the Rank process from Simuzilla...");

    // Initialize the dispatcher.
    _logger->info("Initializing the dispatcher...");
    _dispatcher = Dispatcher::get_instance([this](const UUIDv4& uuid, uint32_t pid = 0) -> void { return mark_origin(uuid, pid); }, _logger->name());
#endif
}
