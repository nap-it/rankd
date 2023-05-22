#ifndef YANP_IT_DDS_NCM_H
#define YANP_IT_DDS_NCM_H

#include <atomic>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>

#include "descriptors/ncmPubSubTypes.h"
#include "passivenp.h"

using namespace eprosima::fastdds::dds;

class NcmPubListener : public DataWriterListener {
public:
    NcmPubListener() : _matched(0) {
    }
    ~NcmPubListener() override {
    }

    void on_publication_matched(DataWriter*, const PublicationMatchedStatus& info) override {
    }

    std::atomic_int _matched;
};

class NcmPublisher {
public:
    NcmPublisher() : _participant(nullptr), _publisher(nullptr), _topic(nullptr), _writer(nullptr) {
    }

    virtual ~NcmPublisher() {
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

    void generate_message(const ncm::Result& result) {
        // Set timestamp.
        unsigned long timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        _message_structure.timestamp(timestamp);

        // Get state from the results.
        Ncm::State state;
        Ncm::Gps state_location;
        state_location.latitude(result.state().location.latitude);
        state_location.longitude(result.state().location.longitude);
        state_location.speed(result.state().location.speed);
        state_location.track(result.state().location.track);
        state.location(state_location);
        Ncm::ConnectionState state_enum;
        switch (result.state().state) {
            case ncm::ConnectionState::DISCONNECTED:
                state_enum = Ncm::ConnectionState::DISCONNECTED;
                break;
            case ncm::ConnectionState::CONNECTED_CELLULAR:
                state_enum = Ncm::ConnectionState::CONNECTED_CELLULAR;
                break;
            case ncm::ConnectionState::CONNECTED_WAVE:
                state_enum = Ncm::ConnectionState::CONNECTED_WAVE;
                break;
            case ncm::ConnectionState::ERRONEOUS_STATE:
                state_enum = Ncm::ConnectionState::ERRONEOUS_STATE;
                break;
        }
        state.connection_state(state_enum);
        state.neighbor_station(result.state().connected_gateway_id);
        _message_structure.state_item(state);

        // Get last handover information from the results.
        Ncm::Handover last_handover;
        Ncm::ConnectionState source_state;
        switch (result.last_handover().state_switching.first) {
            case ncm::ConnectionState::DISCONNECTED:
                source_state = Ncm::ConnectionState::DISCONNECTED;
                break;
            case ncm::ConnectionState::CONNECTED_CELLULAR:
                source_state = Ncm::ConnectionState::CONNECTED_CELLULAR;
                break;
            case ncm::ConnectionState::CONNECTED_WAVE:
                source_state = Ncm::ConnectionState::CONNECTED_WAVE;
                break;
            case ncm::ConnectionState::ERRONEOUS_STATE:
                source_state = Ncm::ConnectionState::ERRONEOUS_STATE;
                break;
        }
        last_handover.source_state(source_state);
        last_handover.source_gateway_id(result.last_handover().gateway_id_switching.first);
        Ncm::ConnectionState destination_state;
        switch (result.last_handover().state_switching.second) {
            case ncm::ConnectionState::DISCONNECTED:
                destination_state = Ncm::ConnectionState::DISCONNECTED;
                break;
            case ncm::ConnectionState::CONNECTED_CELLULAR:
                destination_state = Ncm::ConnectionState::CONNECTED_CELLULAR;
                break;
            case ncm::ConnectionState::CONNECTED_WAVE:
                destination_state = Ncm::ConnectionState::CONNECTED_WAVE;
                break;
            case ncm::ConnectionState::ERRONEOUS_STATE:
                destination_state = Ncm::ConnectionState::ERRONEOUS_STATE;
                break;
        }
        last_handover.destination_state(destination_state);
        last_handover.destination_gateway_id(result.last_handover().gateway_id_switching.second);
        Ncm::HandoverType last_handover_type;
        switch (result.last_handover().type) {
            case ncm::HandoverType::SLOW_HANDOVER:
                last_handover_type = Ncm::SLOW_HANDOVER;
                break;
            case ncm::HandoverType::REGULAR_HANDOVER:
                last_handover_type = Ncm::REGULAR_HANDOVER;
                break;
            case ncm::HandoverType::ERRONEOUS_HANDOVER:
                last_handover_type = Ncm::ERRONEOUS_HANDOVER;
                break;
        }
        last_handover.type(last_handover_type);
        last_handover.average_signal(result.last_handover().average_signal);
        last_handover.azimuth(result.last_handover().azimuth);
        Ncm::Gps last_handover_location;
        last_handover_location.latitude(result.last_handover().location.latitude);
        last_handover_location.longitude(result.last_handover().location.longitude);
        last_handover_location.speed(result.last_handover().location.speed);
        last_handover_location.track(result.last_handover().location.track);
        last_handover.location(last_handover_location);
        _message_structure.handover_item(last_handover);

        // Add neighbors information from the results.
        std::vector<Ncm::NeighborStation> stations;
        for (const auto& [neighbor_id, neighbor] : result.neighbors()) {
            Ncm::NeighborStation station;
            station.identification(neighbor.identification);
            Ncm::Gps station_location;
            station_location.latitude(neighbor.location.longitude);
            station_location.longitude(neighbor.location.longitude);
            station_location.speed(neighbor.location.speed);
            station_location.track(neighbor.location.track);
            station.location(station_location);
            station.in_range(neighbor.in_range);
            station.timestamp(neighbor.timestamp);
            station.signal(neighbor.signal);
            station.average_signal(neighbor.average_signal);
            station.azimuth(neighbor.azimuth);
            station.tx_rate(neighbor.tx_rate);
            station.inactive_time(neighbor.inactive_time);
            station.i_throughput(neighbor.i_throughput);
            station.d_throughput(neighbor.d_throughput);
            station.rx_bytes(neighbor.rx_bytes);
            station.tx_bytes(neighbor.tx_bytes);
            station.tx_retries(neighbor.tx_retries);
            station.tx_fails(neighbor.tx_fails);
            stations.push_back(station);
        }
        _message_structure.stations(stations);
    }

    bool execute() {
        // Define the participant-level entity.
        DomainParticipantQos participant_qos;
        participant_qos.name("Ncm_publisher");
        _participant = DomainParticipantFactory::get_instance()->create_participant(0, participant_qos);
        if (_participant == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Crate the publication topic.
        _topic = _participant->create_topic("yanpit/ncm", "YanpitNcm", TOPIC_QOS_DEFAULT);
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
    Ncm::NcmStatus _message_structure;
    DomainParticipant* _participant;
    Publisher* _publisher;
    Topic* _topic;
    DataWriter* _writer;
    NcmPubListener _listener;
};

#endif  // YANP_IT_DDS_NCM_H
