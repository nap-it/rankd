#include "structs/dispatchers/receiver_dds.h"

//void ReceiverDDS::enqueue_bytes(const std::vector<uint8_t> &bytes) {
//    _queue.push(bytes);
//}

void ReceiverDDS::set_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>> *queue, std::mutex *mutex) {
    _message_deposit = queue;
    _message_deposit_mutex = mutex;
}

ReceiverDDS *ReceiverDDS::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

ReceiverDDS *ReceiverDDS::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool ReceiverDDS::is_running() {
    return _running;
}

void ReceiverDDS::operator()() {
    while (_running) {
        if (not _queue.empty()) {
            std::pair<std::vector<uint8_t>, std::vector<uint8_t>> source_and_bytes;
            source_and_bytes = _queue.front();
            _queue.pop();

            // TODO Parse the "bytes" into a message.
            Message* message = parse_message_from_bytes(source_and_bytes.second, false);
            {
                std::lock_guard guard(*_message_deposit_mutex);
                _message_deposit->emplace(message, source_and_bytes.first, IdentifierType::DDS);
            }
        }
    }
}