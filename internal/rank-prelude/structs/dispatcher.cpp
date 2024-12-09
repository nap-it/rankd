#include "structs/dispatcher.h"

Dispatcher *Dispatcher::execute_dispatchers() {
    _logger->trace("[Dispatcher] Executing all the receivers.");
#ifdef FROM_SIMUZILLA
    _raw_receiver_simulation->execute();
#else
    _raw_receiver_l2->execute();
    _receiver_l3->execute();
    _receiver_dds->execute();
#endif
    //_sender->execute();

    return this;
}

Dispatcher *Dispatcher::stop_dispatchers() {
    _logger->trace("[Dispatcher] Stopping all the dispatching units...");
#ifdef FROM_SIMUZILLA
    _raw_receiver_simulation->stop();
#else
    _receiver_l2->stop();
    _receiver_l3->stop();
    _receiver_dds->stop();
#endif
    //_sender->stop();

    return this;
}

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

#ifdef FROM_SIMUZILLA
void Dispatcher::set_topology_and_current_address(std::function<const std::vector<int>*()> topology, unsigned int address) {
    _sender->set_topology_and_current_address(topology, address);
}
#endif

Dispatcher::Dispatcher() {
    _logger->info("Preparing the dispatcher unit...");

    // Set queues and respective mutexes for synchronization on accesses.
    _logger->trace("[Dispatcher] Setting the depositing queue in simulator receiver.");
    _receiver_simulation->set_message_deposit_queue(_received_messages, &_received_messages_locker);

#ifdef FROM_SIMUZILLA
    auto sim_queue = _raw_receiver_simulation->queue_access();
    _logger->trace("[Dispatcher] Setting an intermediate queue between raw-simulator and simulator receivers.");
    _receiver_simulation->set_queue(sim_queue.first, sim_queue.second);
#else
    _logger->trace("[Dispatcher] Setting the depositing queue in L2 receiver.");
    _receiver_l2->set_message_deposit_queue(_received_messages, &_received_messages_locker);
    auto l2_queue = _raw_receiver_l2->queue_access();
    _logger->trace("[Dispatcher] Setting an intermediate queue between raw-L2 and L2 receivers.");
    _receiver_l2->set_queue(l2_queue.first, l2_queue.second);

    _logger->trace("[Dispatcher] Setting the depositing queue in L3 receiver.");
    _receiver_l3->set_queue(_received_messages, &_received_messages_locker);

    _logger->trace("[Dispatcher] Setting the depositing queue in DDS receiver.");
    _receiver_dds->set_queue(_received_messages, &_received_messages_locker);
#endif

    _logger->trace("[Dispatcher] Setting the sending queue in sender component.");
    _sender->set_queue(_sending_messages, &_sending_messages_locker);

    // Borrow the control of the receivers to the raw receivers.
    _logger->trace("[Dispatcher] Borrow the control of the receivers to the raw receivers.");

#ifdef FROM_SIMUZILLA
    _raw_receiver_simulation->receive_control_borrowing_from(_receiver_simulation);
#else
    _raw_receiver_l2->receive_control_borrowing_from(_receiver_l2);
#endif
}

#ifdef FROM_SIMUZILLA
Dispatcher *Dispatcher::borrow_simulation_receiver_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>()> function) {
    _logger->trace("[Dispatcher] Registering Rx function in raw receiver.");
    _raw_receiver_simulation->borrow_receiver_function(std::move(function));

    return this;
}

Dispatcher *Dispatcher::borrow_simulation_sender_function(std::function<void(uint8_t, std::vector<uint8_t>)> function) {
    _logger->trace("[Dispatcher] Registering Tx function in sender.");
    _sender->borrow_sender_function(function);

    return this;
}
#endif

Dispatcher::~Dispatcher() {
    _logger->trace("[Dispatcher] Stopping all the dispatching units...");
#ifdef FROM_SIMUZILLA
    _raw_receiver_simulation->stop();
#else
    _receiver_l2->stop();
    _receiver_l3->stop();
    _receiver_dds->stop();
#endif
    _sender->stop();
}