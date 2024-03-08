#include "structs/dispatchers/receiver_l3.h"

//void ReceiverL3::enqueue_bytes(const std::vector<uint8_t> &bytes) {
//    _queue.push(bytes);
//}

void ReceiverL3::set_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>> *queue, std::mutex *mutex) {
    _message_deposit = queue;
    _message_deposit_mutex = mutex;
}

ReceiverL3 *ReceiverL3::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

ReceiverL3 *ReceiverL3::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool ReceiverL3::is_running() {
    return _running;
}

void ReceiverL3::operator()() {
    while (_running) {
        if (not _queue.empty()) {
            std::pair<std::vector<uint8_t>, std::vector<uint8_t>> source_and_bytes;
            source_and_bytes = _queue.front();
            _queue.pop();

            // TODO Parse the "bytes" into a message.
            Message* message = parse_message_from_bytes(source_and_bytes.second, true);
            {
                std::lock_guard guard(*_message_deposit_mutex);
                if (source_and_bytes.first.size() == IPV4_ADDR_LEN) {
                    _message_deposit->emplace(message, source_and_bytes.first, IdentifierType::IPv4);
                } else if (source_and_bytes.first.size() == IPV6_ADDR_LEN) {
                    _message_deposit->emplace(message, source_and_bytes.first, IdentifierType::IPv6);
                }
            }
        }
    }
}