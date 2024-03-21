#include "structs/dispatchers/receiver_simulation.h"

std::pair<std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> *, std::mutex *>
RawReceiverSimulation::queue_access() {
    return std::make_pair(_queue, _queue_mutex);
}

RawReceiverSimulation *RawReceiverSimulation::receive_control_borrowing_from(ReceiverSimulation *controller) {
    _receiver_controller = controller;

    return this;
}

RawReceiverSimulation *RawReceiverSimulation::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

RawReceiverSimulation *RawReceiverSimulation::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool RawReceiverSimulation::is_running() {
    return _running;
}

RawReceiverSimulation::RawReceiverSimulation() {}

RawReceiverSimulation::~RawReceiverSimulation() {}

void RawReceiverSimulation::operator()() {
    while (_running) {
        // Receive a message from the recv function.
        std::pair<uint8_t, std::vector<uint8_t>> data{};
#ifndef SIMUZILLA
        data = (*(static_cast<std::function<std::pair<uint8_t, std::vector<uint8_t>>(void)>*>(_simulated_recv)))();
#endif

        // If the data response is empty, ignore it.
        if (data.second.empty()) {
            continue;
        }

        // Get origin from the first item in response.
        std::vector<uint8_t> source_identifier = {data.first};

        // Add such data to the queue to be handled.
        std::lock_guard<std::mutex> guard(*_queue_mutex);
        _queue->emplace(source_identifier, data.second);

        // Execute the Receiver to handle such messages.
        _receiver_controller->execute();
    }
}

#ifndef SIMUZILLA
RawReceiverSimulation *
RawReceiverSimulation::borrow_receiver_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>()> *function) {
    _simulated_recv = function;

    return this;
}
#endif

void ReceiverSimulation::set_queue(std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> *queue, std::mutex *mutex) {
    _queue = queue;
    _queue_mutex = mutex;
}

void ReceiverSimulation::set_message_deposit_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>> *queue, std::mutex *mutex) {
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
        if (not _queue->empty()) {
            std::pair<std::vector<uint8_t>, std::vector<uint8_t>> source_and_bytes;
            {
                std::lock_guard<std::mutex> guard(*_queue_mutex);
                source_and_bytes = _queue->front();
                _queue->pop();
            }

            Message* message = parse_message_from_bytes(source_and_bytes.second, false);
            if (message == nullptr) {
                // TODO Handle this error, since it means that this message will be ignored.
                continue;
            }
            {
                std::lock_guard guard(*_message_deposit_mutex);
                _message_deposit->emplace(message, source_and_bytes.first, IdentifierType::Simulation);
            }
        } else {
            stop();
        }
    }
}