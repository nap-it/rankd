#include "structs/dispatchers/sender.h"

Sender *Sender::set_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* queue, std::mutex *mutex) {
    _queue = queue;
    _queue_mutex = mutex;

    return this;
}

Sender *Sender::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

Sender *Sender::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool Sender::is_running() {
    return _running;
}

void Sender::operator()() {
    while (_running) {
        if (not _queue->empty()) {
            std::tuple<Message*, std::vector<uint8_t>, IdentifierType> queue_item{};
            {
                std::lock_guard<std::mutex> guard(*_queue_mutex);
                queue_item = _queue->front();
                _queue->pop();
            }

            switch (std::get<2>(queue_item)) {
                case IdentifierType::MAC:
                    // The target is a 6-bytes address: MAC.
                    make_and_send_frame(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
                case IdentifierType::IPv4:
                    // The target is a 4-bytes address: IPv4.
                    make_and_send_packet(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
                case IdentifierType::IPv6:
                    // The target is a 16-bytes address: IPv6.
                    make_and_send_packet(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
                case IdentifierType::DDS:
                    // The target is a 16-bytes address: DDS.
                    make_and_send_message(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
                case IdentifierType::Simulation:
                    // The target is a simulated unit: Simuzilla, for instance.
                    make_and_send_bytes(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
            }
        } else {
            stop();
        }
    }
}

Sender::Sender() {

}

void Sender::make_and_send_frame(const Message *message, const std::vector<uint8_t> &target) const {
    // Get source address of the interface from which the target is reachable? Only Ethernet interfaces?

}

void Sender::make_and_send_packet(const Message *message, const std::vector<uint8_t> &target) const {

}

void Sender::make_and_send_message(const Message *message, const std::vector<uint8_t> &target) const {

}

void Sender::make_and_send_bytes(const Message *message, const std::vector<uint8_t> &target) const {

}