#include "structs/dispatcher.h"

void Dispatcher::send_message(Message* message, const std::vector<uint8_t>& target, const IdentifierType& type) {
    {
        std::lock_guard<std::mutex> guard(_sending_messages_locker);
        _sending_messages->emplace(message, target, type);
    }

    _sender->execute();
}

bool Dispatcher::receiving_queue_is_empty() const {
    return _received_messages->empty();
}

bool Dispatcher::receiving_queue_has_message() const {
    return not receiving_queue_is_empty();
}

std::tuple<Message*, std::vector<uint8_t>, IdentifierType> Dispatcher::dequeue_item() {
    std::tuple<Message*, std::vector<uint8_t>, IdentifierType> message_source_and_type;

    {
        std::lock_guard guard(_received_messages_locker);

        message_source_and_type = _received_messages->front();
        _received_messages->pop();
    }

    return message_source_and_type;
}

Dispatcher::Dispatcher() {
    // Set queues and respective mutexes for synchronization on accesses.
    _receiver_simulation->set_message_deposit_queue(_received_messages, &_received_messages_locker);
    auto sim_queue = _raw_receiver_simulation->queue_access();
    _receiver_simulation->set_queue(sim_queue.first, sim_queue.second);
    _receiver_l2->set_message_deposit_queue(_received_messages, &_received_messages_locker);
    auto l2_queue = _raw_receiver_l2->queue_access();
    _receiver_l2->set_queue(l2_queue.first, l2_queue.second);
    _receiver_l3->set_queue(_received_messages, &_received_messages_locker);
    _receiver_dds->set_queue(_received_messages, &_received_messages_locker);
    _sender->set_queue(_sending_messages, &_sending_messages_locker);

    // Borrow the control of the L2 receiver to the raw receiver.
    _raw_receiver_l2->receive_control_borrowing_from(_receiver_l2);
    _raw_receiver_simulation->receive_control_borrowing_from(_receiver_simulation);

    // Run the receivers.
    _raw_receiver_simulation->execute();
    _raw_receiver_l2->execute();
    _receiver_l3->execute();
    _receiver_dds->execute();
}

#ifndef SIMUZILLA
Dispatcher *Dispatcher::borrow_simulation_receiver_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>()> *function) {
    _raw_receiver_simulation->borrow_receiver_function(function);

    return this;
}

Dispatcher *Dispatcher::borrow_simulation_sender_function(std::function<void(uint8_t, const std::vector<uint8_t> &)> *function) {
    _sender->borrow_sender_function(function);

    return this;
}
#endif

Dispatcher::~Dispatcher() {
    _receiver_simulation->stop();
    _receiver_l2->stop();
    _receiver_l3->stop();
    _receiver_dds->stop();
}