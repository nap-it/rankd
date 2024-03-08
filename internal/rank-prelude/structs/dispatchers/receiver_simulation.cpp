#include "structs/dispatchers/receiver_simulation.h"

//void ReceiverSimulation::enqueue_bytes(const std::vector<uint8_t> &bytes) {
//    _queue.push(bytes);
//}

void ReceiverSimulation::set_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>> *queue, std::mutex *mutex) {
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
            std::pair<std::vector<uint8_t>, std::vector<uint8_t>> source_and_bytes;
            source_and_bytes = _queue.front();
            _queue.pop();

            // TODO Parse the "bytes" into a message.
            Message* message = parse_message_from_bytes(source_and_bytes.second, false);
            {
                std::lock_guard guard(*_message_deposit_mutex);
                _message_deposit->emplace(message, source_and_bytes.first, IdentifierType::Simulation);
            }
        }
    }
}