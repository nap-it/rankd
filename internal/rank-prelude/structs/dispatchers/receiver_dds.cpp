#include "structs/dispatchers/receiver_dds.h"

void ReceiverDDS::enqueue_bytes(const std::vector<uint8_t> &bytes) {
    _queue.push(bytes);
}

void ReceiverDDS::set_queue(std::queue<Message *> *queue, std::mutex *mutex) {
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
            auto bytes = _queue.front();
            _queue.pop();

            // TODO Parse the "bytes" into a message.
            Message* message = nullptr;
            {
                std::lock_guard guard(*_message_deposit_mutex);
                _message_deposit->push(message);
            }
        }
    }
}