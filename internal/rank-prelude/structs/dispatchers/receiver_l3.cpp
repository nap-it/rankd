#include "structs/dispatchers/receiver_l3.h"

void ReceiverL3::enqueue_bytes(const std::vector<uint8_t> &bytes) {
    _queue.push(bytes);
}

void ReceiverL3::set_queue(std::queue<Message *> *queue, std::mutex *mutex) {
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