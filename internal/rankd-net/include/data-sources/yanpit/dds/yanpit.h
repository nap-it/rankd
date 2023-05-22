#ifndef YANP_IT_DDS_YANPIT_H
#define YANP_IT_DDS_YANPIT_H

#include <atomic>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>

#include "descriptors/yanpitPubSubTypes.h"

using namespace eprosima::fastdds::dds;

class YanpitPubListener : public DataWriterListener {
public:
    YanpitPubListener() : _matched(0) {
    }
    ~YanpitPubListener() override {
    }

    void on_publication_matched(DataWriter*, const PublicationMatchedStatus& info) override {
    }

    std::atomic_int _matched;
};

class YanpitPublisher {
public:
    YanpitPublisher() : _participant(nullptr), _publisher(nullptr), _topic(nullptr), _writer(nullptr) {
    }

    virtual ~YanpitPublisher() {
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

    void generate_message(const std::map<char, std::map<std::string, unsigned int>>& statuses) {
        // Set Active Tools.
        Yanpit::ActiveTools active_tools;
        Yanpit::IperfStatus iperf_status;
        iperf_status.running_instances(statuses.at('A').at("iperf"));
        active_tools.iperf(iperf_status);

        // Set passive tools.
        Yanpit::PassiveTools passive_tools;
        Yanpit::LocalnetStatus localnet_status;
        localnet_status.is_running(statuses.at('P').at("localnet"));
        passive_tools.localnet(localnet_status);
        Yanpit::NcmStatus ncm_status;
        ncm_status.is_running(statuses.at('P').at("ncm"));
        passive_tools.ncm(ncm_status);

        // Set Yanp-it object.
        _message_structure.active_tools(active_tools);
        _message_structure.passive_tools(passive_tools);
    }

    bool execute() {
        // Define the participant-level entity.
        DomainParticipantQos participant_qos;
        participant_qos.name("Yanpit_publisher");
        _participant = DomainParticipantFactory::get_instance()->create_participant(0, participant_qos);
        if (_participant == nullptr) {
            // TODO Handle this error.
            return false;
        }

        // Crate the publication topic.
        _topic = _participant->create_topic("yanpit/status", "YanpitStatus", TOPIC_QOS_DEFAULT);
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
    Yanpit::YanpitStatus _message_structure;
    DomainParticipant* _participant;
    Publisher* _publisher;
    Topic* _topic;
    DataWriter* _writer;
    YanpitPubListener _listener;
};

#endif  // YANP_IT_DDS_YANPIT_H
