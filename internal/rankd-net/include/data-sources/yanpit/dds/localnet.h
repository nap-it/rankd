#ifndef YANP_IT_DDS_LOCALNET_H
#define YANP_IT_DDS_LOCALNET_H

#include <atomic>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>

#include "passivenp.h"
#include "descriptors/localnetPubSubTypes.h"

using namespace eprosima::fastdds::dds;

class LocalnetPubListener : public DataWriterListener {
public:
    LocalnetPubListener() : _matched(0) {
    }
    ~LocalnetPubListener() override {
    }

    void on_publication_matched(DataWriter*, const PublicationMatchedStatus& info) override {
    }

    std::atomic_int _matched;
};

class LocalnetPublisher {
public:
    LocalnetPublisher() : _participant(nullptr), _publisher(nullptr), _topic(nullptr), _writer(nullptr) {
    }

    virtual ~LocalnetPublisher() {
        if (_writer != nullptr) {
            _publisher->delete_datawriter(_writer);
        }
        if (_publisher != nullptr) {
            _participant->delete_publisher(_publisher);
        }
        if (_topic != nullptr) {
            _participant->delete_topic(_topic);
        }

        DomainParticipantFactory::get_instance()->delete_participant(_participant);
    }

    void generate_message(const std::map<std::string, localnet::NetworkInterface>& interfaces) {
        // Set timestamp.
        unsigned long timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        _message_structure.timestamp(timestamp);

        // Set interfaces.
        for (const auto& [interface_name, interface_item] : interfaces) {
            Localnet::NetworkInterface interface;
            interface.mac_address(interface_item.name());
            interface.ip4_addresses(interface_item.ip_4_addresses());
            interface.ip6_addresses(interface_item.ip_6_addresses());
            interface.has_carrier(interface_item.has_carrier());
            interface.duplex_type(parse_duplex_type(interface_item.duplex()));
            interface.interface_index(interface_item.interface_index());
            interface.enslaved_interface_index(interface_item.enslaved_interface_index());
            interface.mtu(interface_item.mtu());
            interface.operation_state(interface_item.operation_state());
            interface.speed(interface_item.speed());
            interface.rx_bytes(interface_item.rx_bytes());
            interface.tx_bytes(interface_item.tx_bytes());
            interface.rx_packets(interface_item.rx_packets());
            interface.tx_packets(interface_item.tx_packets());
            interface.rx_errors(interface_item.rx_errors());
            interface.tx_errors(interface_item.tx_errors());
            interface.rx_dropped(interface_item.rx_dropped());
            interface.tx_dropped(interface_item.tx_dropped());

            Localnet::MapEntry map_entry;
            map_entry.interface_name(interface_name);
            map_entry.interface_item(interface);

            _message_structure.interfaces().push_back(map_entry);
        }
    }

    bool execute() {
        // Define the participant-level entity.
        DomainParticipantQos participant_qos;
        participant_qos.name("Localnet_publisher");
        _participant = DomainParticipantFactory::get_instance()->create_participant(0, participant_qos);
        if (_participant == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Crate the publication topic.
        _topic = _participant->create_topic("yanpit/localnet", "YanpitLocalnet", TOPIC_QOS_DEFAULT);
        if (_topic == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Create the publisher entity.
        _publisher = _participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (_publisher == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Set up the writer to perform publications.
        _writer = _publisher->create_datawriter(_topic, DATAWRITER_QOS_DEFAULT, &_listener);
        if (_writer == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Start the class inner thread.
        _is_running = true;
        _thread = std::thread(std::ref(*this));

        return true;
    }

    void operator()() {
        while (_is_running) {

            if (_listener._matched > 0) {
                _writer->write(&_message_structure);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    void stop() {
        if (!_is_running) {
            return;
        }
        _is_running = false;
        _thread.join();
    }

private:
    std::atomic<bool> _is_running = false;
    std::thread _thread;
    Localnet::LocalnetStatus _message_structure;
    DomainParticipant* _participant;
    Publisher* _publisher;
    Topic* _topic;
    DataWriter* _writer;
    LocalnetPubListener _listener;
};

#endif  // YANP_IT_DDS_LOCALNET_H
