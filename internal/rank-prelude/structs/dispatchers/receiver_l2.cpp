#include "structs/dispatchers/receiver_l2.h"

std::pair<std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> *, std::mutex *> RawReceiverL2::queue_access() {
    return std::make_pair(_queue, _queue_mutex);
}

RawReceiverL2 *RawReceiverL2::receive_control_borrowing_from(ReceiverL2 *controller) {
    _receiver_controller = controller;

    return this;
}

RawReceiverL2 *RawReceiverL2::execute() {
    _logger->trace("[RawReceiverL2] Executing a RawReceiverSimulation...");
    if (_running) {
        _logger->warn("[RawReceiverL2] A raw receiver for L2 was already running. Someone is calling for execution again.");
        return this;
    }

    _logger->trace("[RawReceiverL2] Starting the main thread...");
    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

RawReceiverL2 *RawReceiverL2::stop() {
    _logger->trace("[RawReceiverL2] Stopping a RawReceiverSimulation...");
    if (not _running) {
        _logger->warn("[RawReceiverL2] A raw receiver for simulation was already stopped. Someone is calling to stop again.");
        return this;
    }

    _logger->trace("[RawReceiverL2] Stopping the main thread...");
    _running = false;
    _thread.join();

    _logger->info("The raw receiver for L2 has been stopped.");

    return this;
}

bool RawReceiverL2::is_running() {
    return _running;
}

RawReceiverL2::RawReceiverL2(const std::string& logger_name) {
    // Configure logging.
    _logger = spdlog::get(logger_name);

    _logger->trace("[RawReceiverL2] Preparing the L2 raw receiver...");

    // Load eBPF program.
    _logger->trace("[RawReceiverL2] Loading the eBPF program into the kernel. (Step 1 of 4)");
    struct rank_l2_bpf* skeleton;
    skeleton = rank_l2_bpf__open_and_load();
    int error = rank_l2_bpf__attach(skeleton);
    // TODO Handle error code with errno.
    struct bpf_map* interface_map = bpf_object__find_map_by_name(skeleton->obj, "interface");
    int zero = 0;
    bpf_map__update_elem(interface_map, &zero, sizeof(int), reinterpret_cast<void*>(if_nametoindex(RANK_INTERFACE)), sizeof(RANK_INTERFACE), BPF_ANY);

    // Create the raw socket.
    _logger->trace("[RawReceiverL2] Create the raw socket with the rank0 interface. (Step 2 of 4)");
    _raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (_raw_socket < 0) {
        _logger->critical("[RawReceiverL2] The raw socket could not be created. Error number {}.", _raw_socket);
        throw std::exception(); // TODO Handle this error. This needs high privileges.
    }

    // Get the interface index.
    _logger->trace("[RawReceiverL2] Getting the rank0 interface index within the system. (Step 3 of 4)");
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, reinterpret_cast<const char*>(RANK_INTERFACE), sizeof(ifr.ifr_name));
    int error_code = ioctl(_raw_socket, SIOCGIFINDEX, &ifr);
    if (error_code < 0) {
        _logger->critical("[RawReceiverL2] The interface index could not be estimated. Error number {}.", error_code);
        throw std::exception(); // TODO Handle this error.
    }
    int interface_index = ifr.ifr_ifindex;
    _logger->debug("[RawReceiverL2] The rank0 interface has interface index {}.", interface_index);

    // Bind raw socket to given network interface.
    _logger->trace("[RawReceiverL2] Binding raw socket to the {} interface index. (Step 4 of 4)", interface_index);
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(struct sockaddr_ll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = interface_index;
    sll.sll_protocol = ETH_P_ALL;
    error_code = bind(_raw_socket, (struct sockaddr*)&sll, sizeof(struct sockaddr_ll));
    if (error_code == -1) {
        _logger->critical("[RawReceiverL2] There was an error attempting to bind the socket to the network interface. Error number {}.", error_code);
        throw std::exception(); // TODO Handle this error.
    }

    // Initialize temporary buffer.
    _temporary_buffer = (unsigned char*) malloc(TEMPORARY_BUFFER_LENGTH);

    _logger->trace("[RawReceiverL2] The L2 raw receiver is ready to run.");
}

RawReceiverL2::~RawReceiverL2() {
    // Free allocated memory.
    free(_temporary_buffer);

    // Close opened raw socket.
    close(_raw_socket);
}

void RawReceiverL2::operator()() {
    _logger->info("Starting the raw receiver for L2 main thread...");
    _logger->info("The raw receiver for L2 is now waiting for messages from other L2 nodes in blocking manner.");

    while (_running) {
        // Clear the temporary buffer completely.
        memset(_temporary_buffer, 0, TEMPORARY_BUFFER_LENGTH);

        // Wait for another frame to be received. This method will be sensible to all Ethernet frames!
        _logger->trace("[RawReceiverL2] Waiting for a message to come…");
        int received_bytes = recv(_raw_socket, _temporary_buffer, TEMPORARY_BUFFER_LENGTH, 0);
        _logger->debug("[RawReceiverL2] Outside --> RawReceiverL2.bytes: {} received bytes.", received_bytes);
        if (received_bytes < 0) {
            _logger->error("[RawReceiverL2] The received bytes were not enough to complete a message. We will ignore this message.");
            continue;
        }

        // Parse the received bytes as an Ethernet header.
        struct ethhdr* ethernet_header = (struct ethhdr*) _temporary_buffer;
        if (ntohs(ethernet_header->h_proto) == RANK_ETHERTYPE) {
            std::vector<uint8_t> data;
            uint8_t* rank_data = _temporary_buffer + sizeof(struct ethhdr);
            size_t rank_data_length = received_bytes - sizeof(struct ethhdr);

            // Insert the sliced C-array into the newly created vector.
            data.insert(data.end(), &rank_data[0], &rank_data[rank_data_length]);

            // Get source MAC address.
            std::vector<uint8_t> source_address(ethernet_header->h_source, ethernet_header->h_source + sizeof ethernet_header->h_source / sizeof ethernet_header->h_source[0]);
            std::transform(source_address.begin(), source_address.end(), source_address.begin(), [](uint8_t byte) { return ntohs(byte); });

            _logger->debug("[RawReceiverL2] The received message came from {:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}.", source_address.at(0), source_address.at(1), source_address.at(2), source_address.at(3), source_address.at(4), source_address.at(5));

            std::lock_guard<std::mutex> guard(*_queue_mutex);
            _queue->emplace(source_address, data);
        } else {
            _logger->debug("[RawReceiverL2] The data bytes do not match with a Rank frame ethertype ({} is different than Rank's {}).", ntohs(ethernet_header->h_proto), RANK_ETHERTYPE);
            continue;
        }
        _logger->trace("[RawReceiverL2] The data bytes were enqueued to the L2 receiver to handle.");

        // Execute the Receiver to handle such messages.
        _logger->trace("[RawReceiverL2] Awakening the receiver for L2 to handle these data.");
        _receiver_controller->execute();
    }
}

void ReceiverL2::set_queue(std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> *queue, std::mutex *mutex) {
    _queue = queue;
    _queue_mutex = mutex;
}

void ReceiverL2::set_message_deposit_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>> *queue, std::mutex *mutex) {
    _message_deposit = queue;
    _message_deposit_mutex = mutex;
}

ReceiverL2 *ReceiverL2::execute() {
    _logger->trace("[ReceiverL2] Executing a ReceiverL2...");
    if (_running) {
        _logger->warn("[ReceiverL2] A receiver for L2 was already running. Someone is calling for execution again.");
        return this;
    }

    _logger->trace("[ReceiverL2] Starting the main thread...");
    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

ReceiverL2 *ReceiverL2::stop() {
    _logger->trace("[ReceiverL2] Stopping a ReceiverL2...");
    if (not _running) {
        _logger->warn("[ReceiverL2] A raw receiver for L2 was already stopped. Someone is calling to stop again.");
        return this;
    }

    _logger->trace("[ReceiverL2] Stopping the main thread...");
    _running = false;
    _thread.join();

    _logger->info("The receiver for simulation has been stopped.");

    return this;
}

bool ReceiverL2::is_running() {
    return _running;
}

ReceiverL2::ReceiverL2(const std::string &logger_name) {
    // Configure logging.
    _logger = spdlog::get(logger_name);
}

void ReceiverL2::operator()() {
    _logger->info("The receiver for L2 was awakened to handle messages that were deposited in the queue.");

    while (_running) {
        if (not _queue->empty()) {
            _logger->trace("[ReceiverL2] The queue still has an item to be handled (a total of {} items).", _queue->size());
            std::pair<std::vector<uint8_t>, std::vector<uint8_t>> source_and_bytes;
            {
                std::lock_guard<std::mutex> guard(*_queue_mutex);
                source_and_bytes = _queue->front();
                _queue->pop();
            }
            _logger->trace("[ReceiverL2] Received a source with {} bytes, and {} message bytes.", source_and_bytes.first.size(), source_and_bytes.second.size());

            Message* message = parse_message_from_bytes(source_and_bytes.second, true);
            _logger->debug("[ReceiverL2] Bytes --> Message(bytes): {} bytes into a {} message.", source_and_bytes.second.size(), (message != nullptr ? "parseable" : "non-parseable"));
            if (message == nullptr) {
                // TODO Handle this error, since it means that this message will be ignored (its size does not fill up a header or the type is recognized).
                _logger->error("[ReceiverL2] Ignoring erroneous message as it was parsed out from data bytes.");
                continue;
            }
            _logger->trace("[ReceiverL2] Enqueueing the parsed message into the depositing queue of the dispatcher.");
            {
                std::lock_guard guard(*_message_deposit_mutex);
                _message_deposit->emplace(message, source_and_bytes.first, IdentifierType::MAC);
            }
            _logger->info("The receiver for L2 have just enqueued a message into the Dispatcher's depositing queue.");
        } else {
            _logger->trace("[ReceiverL2] The receiver for L2 does not have any item to handle in its queue. Stopping function.");
            stop();
            _logger->info("The receiver for L2 has now ceased functions as there is no data waiting to be handled.");
        }
    }
}