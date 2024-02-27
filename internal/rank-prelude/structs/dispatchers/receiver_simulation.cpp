#include "structs/dispatchers/receiver_simulation.h"

void ReceiverSimulation::enqueue_bytes(const std::vector<uint8_t> &bytes) {
    _queue.push(bytes);
}

void ReceiverSimulation::set_queue(std::queue<Message *> *queue, std::mutex *mutex) {
    _message_deposit = queue;
    _message_deposit_mutex = mutex;
}

ReceiverSimulation *ReceiverSimulation::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

ReceiverSimulation *ReceiverSimulation::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool ReceiverSimulation::is_running() {
    return _running;
}

void ReceiverSimulation::operator()() {
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