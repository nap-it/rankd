#include "structs/dispatchers/sender.h"
#include "structs/messages/ear.h"

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

#ifndef SIMUZILLA
Sender *Sender::borrow_sender_function(std::function<void(uint8_t, const std::vector<uint8_t> &)> *function) {
    _simulated_send = function;

    return this;
}
#endif

Sender::Sender() {

}

void Sender::make_and_send_frame(Message *message, const std::vector<uint8_t> &target) {
    // Get source address of the interface from which the target is reachable? Only Ethernet interfaces?

    // Get current L2 neighbors.
    _neighbors_data_source.snap();
    std::vector<NetworkNeighbor> neighbors = _neighbors_data_source.neighbors();

    // Cast our target from vector of unsigned ints to string for comparison.
    char stringified_target[18];
    sprintf(stringified_target, "%02x:%02x:%02x:%02x:%02x:%02x", target.at(0), target.at(1), target.at(2), target.at(3), target.at(4), target.at(5));

    // Iterate over the neighbors looking for the stringified_target.
    int target_interface_index = -1;
    for (const auto& neighbor : neighbors) {
        if (strcmp(neighbor.l2_address.c_str(), stringified_target) == 0) {
            target_interface_index = neighbor.interface_index;
        }
    }

    // If there is no interface update, then no neighbor was found, the send function should quit.
    if (target_interface_index == -1) {
        // TODO Handle this case.
        return;
    }

    // Get interface name from interface index.
    char interface_name[IFNAMSIZ];
    if_indextoname(target_interface_index, interface_name);

    // Prepare message serialization.
    std::vector<uint8_t> serialized_message;
    switch (message->type()) {
        case MessageType::EAR:
            serialized_message = dynamic_cast<EAR*>(message)->raw_payload();
            break;
        case MessageType::MAR:
            serialized_message = dynamic_cast<MAR*>(message)->raw_payload();
            break;
        case MessageType::BID:
            serialized_message = dynamic_cast<BID*>(message)->raw_payload();
            break;
        case MessageType::ACC:
            serialized_message = dynamic_cast<ACC*>(message)->raw_payload();
            break;
        case MessageType::REF:
            serialized_message = dynamic_cast<REF*>(message)->raw_payload();
            break;
        case MessageType::REP:
            serialized_message = dynamic_cast<REP*>(message)->raw_payload();
            break;
        case MessageType::NOTYPE:
            // TODO Handle this case.
            break;
    }

    // Open the device to send message.
    pcpp::PcapLiveDevice* device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(interface_name);

    // Create Rank L2 packet.
    pcpp::EthLayer ethernet_header(device->getMacAddress(), pcpp::MacAddress(stringified_target), RANK_ETHERTYPE);
    pcpp::PayloadLayer payload(serialized_message.data(), serialized_message.size(), false);
    pcpp::Packet packet;
    packet.addLayer(&ethernet_header);
    packet.addLayer(&payload);
    packet.computeCalculateFields();

    // Send Rank L2 packet.
    device->sendPacket(&packet);

    // Close the device.
    device->close();
}

void Sender::make_and_send_packet(Message *message, const std::vector<uint8_t> &target) const {

}

void Sender::make_and_send_message(Message *message, const std::vector<uint8_t> &target) const {

}

void Sender::make_and_send_bytes(Message *message, const std::vector<uint8_t> &target) const {
    // Get target identifier as a proper simulation identifier.
    uint8_t target_identifier = target.at(0);

    // Prepare message serialization.
    std::vector<uint8_t> serialized_message;
    switch (message->type()) {
        case MessageType::EAR:
            serialized_message = dynamic_cast<EAR*>(message)->raw_payload();
            break;
        case MessageType::MAR:
            serialized_message = dynamic_cast<MAR*>(message)->raw_payload();
            break;
        case MessageType::BID:
            serialized_message = dynamic_cast<BID*>(message)->raw_payload();
            break;
        case MessageType::ACC:
            serialized_message = dynamic_cast<ACC*>(message)->raw_payload();
            break;
        case MessageType::REF:
            serialized_message = dynamic_cast<REF*>(message)->raw_payload();
            break;
        case MessageType::REP:
            serialized_message = dynamic_cast<REP*>(message)->raw_payload();
            break;
        case MessageType::NOTYPE:
            // TODO Handle this case.
            break;
    }

    // Send this message through simulated send function.
#ifndef SIMUZILLA
    (*(static_cast<std::function<void(uint8_t, const std::vector<uint8_t> &)>*>(_simulated_send)))(target_identifier, serialized_message);
#endif
}