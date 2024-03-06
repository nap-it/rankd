#include "structs/dispatcher.h"

size_t Dispatcher::send_bytes(const std::vector<uint8_t> &bytes) {

}

bool Dispatcher::receiving_queue_is_empty() const {
    return _received_messages->empty();
}

Message* Dispatcher::dequeue_message() {
    Message* message;

    {
        std::lock_guard guard(_received_messages_locker);

        message = _received_messages->front();
        _received_messages->pop();
    }

    return message;
}

Dispatcher::Dispatcher() {
    // Set queues and respective mutexes for synchronization on accesses.
    _receiver_simulation->set_queue(_received_messages, &_received_messages_locker);
    _receiver_l2->set_message_deposit_queue(_received_messages, &_received_messages_locker);
    auto l2_queue = _raw_receiver_l2->queue_access();
    _receiver_l2->set_queue(l2_queue.first, l2_queue.second);
    _receiver_l3->set_queue(_received_messages, &_received_messages_locker);
    _receiver_dds->set_queue(_received_messages, &_received_messages_locker);

    // Borrow the control of the L2 receiver to the raw receiver.
    _raw_receiver_l2->receive_control_borrowing_from(_receiver_l2);

    // Run the receivers.
    _receiver_simulation->execute();
    _raw_receiver_l2->execute();
    _receiver_l3->execute();
    _receiver_dds->execute();
}

Dispatcher::~Dispatcher() {
    _receiver_simulation->stop();
    _receiver_l2->stop();
    _receiver_l3->stop();
    _receiver_dds->stop();
}