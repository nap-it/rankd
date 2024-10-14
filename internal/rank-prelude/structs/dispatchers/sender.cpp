#include "structs/dispatchers/sender.h"
#include "structs/messages/ear.h"

Sender *Sender::set_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* queue, std::mutex *mutex) {
    _queue = queue;
    _queue_mutex = mutex;

    return this;
}

Sender *Sender::execute() {
    _logger->trace("[Sender] Executing a Sender...");
    if (_running) {
        _logger->warn("[Sender] A sender was already running. Someone is calling for execution again.");
        return this;
    }

    _logger->trace("[Sender] Starting the main thread...");
    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

Sender *Sender::stop() {
    _logger->trace("[Sender] Stopping a Sender...");
    if (not _running) {
        _logger->warn("[Sender] A sender was already stopped. Someone is calling to stop again.");
        return this;
    }

    _logger->trace("[Sender] Stopping the main thread...");
    _running = false;
    _thread.join();

    _logger->info("The sender has been stopped.");

    return this;
}

bool Sender::is_running() {
    return _running;
}

void Sender::operator()() {
    _logger->info("The sender was awakened to send messages to outside Rank.");

    while (_running) {
        if (not _queue->empty()) {
            _logger->trace("[Sender] The queue still has an item to be handled (a total of {} items).", _queue->size());

            std::tuple<Message*, std::vector<uint8_t>, IdentifierType> queue_item{};
            {
                std::lock_guard<std::mutex> guard(*_queue_mutex);
                queue_item = _queue->front();
                _queue->pop();
            }
            _logger->trace("[Sender] Received a destination in {} with {} bytes, and a {} message.", identifier_to_string(std::get<2>(queue_item)), std::get<1>(queue_item).size(), (std::get<0>(queue_item) != nullptr ? "parseable" : "non-parseable"));

            switch (std::get<2>(queue_item)) {
                case IdentifierType::MAC:
                    // The target is a 6-bytes address: MAC.
                    _logger->trace("[Sender] The target is a MAC address, so delegate this to make and send frame function.");
                    make_and_send_frame(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
                case IdentifierType::IPv4:
                    // The target is a 4-bytes address: IPv4.
                    _logger->trace("[Sender] The target is an IPv4 address, so delegate this to make and send packet function.");
                    make_and_send_packet(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
                case IdentifierType::IPv6:
                    // The target is a 16-bytes address: IPv6.
                    _logger->trace("[Sender] The target is an IPv6 address, so delegate this to make and send packet function.");
                    make_and_send_packet(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
                case IdentifierType::DDS:
                    // The target is a 16-bytes address: DDS.
                    _logger->trace("[Sender] The target is a DDS' RTPS-GUID address, so delegate this to make and send message function.");
                    make_and_send_message(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
                case IdentifierType::Simulation:
                    // The target is a simulated unit: Simuzilla, for instance.
                    _logger->trace("[Sender] The target is a simulation identifier, so delegate this to make and send bytes function.");
                    make_and_send_bytes(std::get<0>(queue_item), std::get<1>(queue_item));
                    break;
            }
        } else {
            _logger->trace("[Sender] The sender does not have any item to handle in its queue. Stopping function.");
            stop();
            _logger->info("The sender has now ceased functions as there is no data waiting to be handled.");
        }
    }
}

#ifdef FROM_SIMUZILLA
void Sender::set_topology_and_current_address(std::function<const std::vector<int>*()> topology, unsigned int address) {
    _topology = topology;
    _own_address = address;
}
#endif

#ifndef SIMUZILLA
Sender *Sender::borrow_sender_function(std::function<void(uint8_t, const std::vector<uint8_t> &)> *function) {
    _simulated_send = function;

    return this;
}
#endif

Sender::Sender() {
    _logger->trace("[Sender] Preparing the sender...");
    _logger->trace("[Sender] The sender is ready to run.");
}

void Sender::make_and_send_frame(Message *message, const std::vector<uint8_t> &target) {
    _logger->trace("[Sender] [Make Frame] Preparing a message in a frame.");

    // Get current L2 neighbors.
    _logger->trace("[Sender] [Make Frame] Getting list of neighbors. (Step 1 of 8)");
    _neighbors_data_source.snap();
    std::vector<NetworkNeighbor> neighbors = _neighbors_data_source.neighbors();
    _logger->debug("[Sender] [Make Frame] Neighbors: {}.", neighbors.size());

    // Cast our target from vector of unsigned ints to string for comparison.
    char stringified_target[18];
    sprintf(stringified_target, "%02x:%02x:%02x:%02x:%02x:%02x", target.at(0), target.at(1), target.at(2), target.at(3), target.at(4), target.at(5));

    // Iterate over the neighbors looking for the stringified_target.
    int target_interface_index = -1;
    for (const auto& neighbor : neighbors) {
        _logger->debug("[Sender] [Make Frame] Comparing {} with targeted {}.", neighbor.l2_address, stringified_target);
        if (strcmp(neighbor.l2_address.c_str(), stringified_target) == 0) {
            target_interface_index = neighbor.interface_index;
            _logger->debug("[Sender] [Make Frame] Found targeted interface index: {}.", target_interface_index);
        }
    }

    // If there is no interface update, then no neighbor was found, the send function should quit.
    if (target_interface_index == -1) {
        _logger->error("[Sender] [Make Frame] The targeted interface was not found. The message could not be sent.");
        // TODO Handle this case.
        return;
    }

    // Get interface name from interface index.
    _logger->trace("[Sender] [Make Frame] Getting the interface name from the targeted interface index. (Step 2 of 8)");
    char interface_name[IFNAMSIZ];
    if_indextoname(target_interface_index, interface_name);
    _logger->debug("[Sender] [Make Frame] Targeted interface name: {}. (Step 3 of 8)", interface_name);

    // Prepare message serialization.
    _logger->trace("[Sender] [Make Frame] Prepare message serialization. (Step 4 of 8)");
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
    _logger->debug("[Sender] [Make Frame] Message in bytes: {} bytes.", serialized_message.size());

    // Open the device to send message.
    _logger->trace("[Sender] [Make Frame] Open the network device to send the message. (Step 5 of 8)");
    pcpp::PcapLiveDevice* device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(interface_name);
    _logger->debug("[Sender] [Make Frame] Network device ({}) status: {}.", interface_name, (device->open() ? "opened" : "closed"));

    // Create Rank L2 frame.
    _logger->trace("[Sender] [Make Frame] Create Rank L2 frame. (Step 6 of 8)");
    pcpp::EthLayer ethernet_header(device->getMacAddress(), pcpp::MacAddress(stringified_target), RANK_ETHERTYPE);
    pcpp::PayloadLayer payload(serialized_message.data(), serialized_message.size(), false);
    pcpp::Packet packet;
    packet.addLayer(&ethernet_header);
    packet.addLayer(&payload);
    packet.computeCalculateFields();

    _logger->debug("[Sender] [Make Frame] Ethernet header length: {} bytes.", ethernet_header.getHeaderLen());
    _logger->debug("[Sender] [Make Frame] Ethernet payload length: {} bytes.", ethernet_header.getDataLen());
    _logger->debug("[Sender] [Make Frame] Serialized message length: {} bytes.", serialized_message.size());

    // Send Rank L2 packet.
    _logger->trace("[Sender] [Make Frame] Send Rank L2 frame. (Step 7 of 8)");
    device->sendPacket(&packet);

    // Close the device.
    _logger->trace("[Sender] [Make Frame] Close the device. (Step 8 of 8)");
    device->close();

    _logger->info("A frame with {} bytes was successfully sent to {} via {}.", serialized_message.size(), stringified_target, interface_name);
}

void Sender::make_and_send_packet(Message *message, const std::vector<uint8_t> &target) const {

}

void Sender::make_and_send_message(Message *message, const std::vector<uint8_t> &target) const {

}

void Sender::make_and_send_bytes(Message *message, const std::vector<uint8_t> &target) const {
    _logger->trace("[Sender] [Make Bytes] Preparing a message in a variable as data bytes.");

    // Get target identifier as a proper simulation identifier.
    _logger->trace("[Sender] [Make Bytes] Get target identifier as a proper simulation identifier.");
    uint8_t target_identifier = target.at(0);

    // Prepare message serialization.
    _logger->trace("[Sender] [Make Bytes] Prepare message serialization.");
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
    _logger->debug("[Sender] [Make Bytes] Message in bytes: {} bytes.", serialized_message.size());

    // Send this message through simulated send function.
#ifndef SIMUZILLA
    _logger->trace("[Sender] [Make Bytes] Send Rank bytes through Simuzilla.");
    (*(static_cast<std::function<void(uint8_t, const std::vector<uint8_t> &)>*>(_simulated_send)))(target_identifier, serialized_message);
    _logger->info("Data with {} bytes were successfully sent to \"Node {}\" via Simuzilla.", serialized_message.size(), target_identifier);
#endif
}