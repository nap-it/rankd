#include "structs/dispatchers/receiver_l2.h"

std::pair<std::queue<std::vector<uint8_t>> *, std::mutex *> RawReceiverL2::queue_access() {
    return std::make_pair(_queue, _queue_mutex);
}

RawReceiverL2 *RawReceiverL2::receive_control_borrowing_from(ReceiverL2 *controller) {
    _receiver_controller = controller;
}

RawReceiverL2 *RawReceiverL2::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

RawReceiverL2 *RawReceiverL2::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool RawReceiverL2::is_running() {
    return _running;
}

RawReceiverL2::RawReceiverL2() {
    // Create the raw socket.
    _raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (_raw_socket < 0) {
        throw std::exception(); // TODO Handle this error. This needs high privileges.
    }

    // Get the interface index.
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, reinterpret_cast<const char*>(RANK_INTERFACE), sizeof(ifr.ifr_name));
    int error_code = ioctl(_raw_socket, SIOCGIFINDEX, &ifr);
    if (error_code < 0) {
        throw std::exception(); // TODO Handle this error.
    }
    int interface_index = ifr.ifr_ifindex;

    // Bind raw socket to given network interface.
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(struct sockaddr_ll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = interface_index;
    sll.sll_protocol = ETH_P_ALL;
    error_code = bind(_raw_socket, (struct sockaddr*)&sll, sizeof(struct sockaddr_ll));
    if (error_code == -1) {
        throw std::exception(); // TODO Handle this error.
    }

    // Initialize temporary buffer.
    _temporary_buffer = (unsigned char*) malloc(TEMPORARY_BUFFER_LENGTH);
}

RawReceiverL2::~RawReceiverL2() {
    // Free allocated memory.
    free(_temporary_buffer);

    // Close opened raw socket.
    close(_raw_socket);
}

void RawReceiverL2::operator()() {
    while (_running) {
        // Clear the temporary buffer completely.
        memset(_temporary_buffer, 0, TEMPORARY_BUFFER_LENGTH);

        // Wait for another frame to be received. This method will be sensible to all Ethernet frames!
        int received_bytes = recv(_raw_socket, _temporary_buffer, TEMPORARY_BUFFER_LENGTH, 0);
        if (received_bytes < 0) {
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

            std::lock_guard<std::mutex> guard(*_queue_mutex);
            _queue->push(data);
        }

        // Execute the Receiver to handle such messages.
        _receiver_controller->execute();
    }
}

void ReceiverL2::enqueue_bytes(const std::vector<uint8_t> &bytes) {
    _queue->push(bytes);
}

void ReceiverL2::set_queue(std::queue<std::vector<uint8_t>> *queue, std::mutex *mutex) {
    _queue = queue;
    _queue_mutex = mutex;
}

void ReceiverL2::set_message_deposit_queue(std::queue<Message *> *queue, std::mutex *mutex) {
    _message_deposit = queue;
    _message_deposit_mutex = mutex;
}

ReceiverL2 *ReceiverL2::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

ReceiverL2 *ReceiverL2::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool ReceiverL2::is_running() {
    return _running;
}

void ReceiverL2::operator()() {
    while (_running) {
        if (not _queue->empty()) {
            std::vector<uint8_t> bytes;
            {
                std::lock_guard<std::mutex> guard(*_queue_mutex);
                bytes = _queue->front();
                _queue->pop();
            }

            // TODO Parse the "bytes" into a message.
            Message* message = nullptr;
            {
                std::lock_guard guard(*_message_deposit_mutex);
                _message_deposit->push(message);
            }
        } else {
            stop();
        }
    }
}