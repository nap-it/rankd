//
// Created by Rui Lopes on 07/06/2023.
//

#ifndef RANKD_NCM_H
#define RANKD_NCM_H

#include "descriptors/ncmPubSubTypes.h"

#include <atomic>
#include <mutex>
#include <thread>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

using namespace eprosima::fastdds::dds;

class NcmSubListener : public DataReaderListener {
public:
    NcmSubListener() {}

    ~NcmSubListener() override {}

    void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override {

    }

    void on_data_available(DataReader* reader) override {

    }

    Ncm::NcmStatus _message;
};

class NcmSubscriber {
public:
    NcmSubscriber() : _participant(nullptr), _subscriber(nullptr), _reader(nullptr), _topic(nullptr) {}

    virtual ~NcmSubscriber() {
        if (_reader != nullptr) {
            _subscriber->delete_datareader(_reader);
        }
        if (_topic != nullptr) {
            _participant->delete_topic(_topic);
        }
        if (_subscriber != nullptr) {
            _participant->delete_subscriber(_subscriber);
        }
        DomainParticipantFactory::get_instance()->delete_participant(_participant);
    }

    void share(Ncm::NcmStatus* data, std::mutex* mutex) {
        _data = data;
        _mutex_pointer = mutex;
    }

    bool execute() {
        DomainParticipantQos participantQos;
        participantQos.name("Participant_subscriber");
        _participant = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);
        if (_participant == nullptr) {
            return false;
        }

        // Create the subscription topic.
        _topic = _participant->create_topic("NcmStatusTopic", "Ncm::NcmStatus", TOPIC_QOS_DEFAULT);
        if (_topic == nullptr) {
            return false;
        }

        // Create the subscriber.
        _subscriber = _participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
        if (_subscriber == nullptr) {
            return false;
        }

        // Create the datareader.
        _reader = _subscriber->create_datareader(_topic, DATAREADER_QOS_DEFAULT, nullptr);
        if (_reader == nullptr) {
            return false;
        }

        // Initiate the thread component.
        _running = true;
        _thread = std::thread(std::ref(*this));

        return true;
    }

    void operator()() {
        while (_running) {
            // TODO
            // FIXME When dealing with _listener, be cautious with the message mutex.

            // Wait for a given pre-defined time range, for another refresh.
            std::this_thread::sleep_for(std::chrono::milliseconds(_thread_sampling_rate));
        }
    }

    bool stop() {
        if (!_running) {
            return true;
        }

        // Stop the threading mechanism.
        _running = false;
        _thread.join();

        return true;
    }
private:
    std::thread _thread;
    std::atomic<bool> _running;
    int _thread_sampling_rate = 100;
    DomainParticipant *_participant;
    Subscriber *_subscriber;
    DataReader *_reader;
    Topic *_topic;
    NcmSubListener _listener;

    // Shared-area with parent thread.
    std::mutex* _mutex_pointer;
    Ncm::NcmStatus* _data;
};

#endif //RANKD_NCM_H
