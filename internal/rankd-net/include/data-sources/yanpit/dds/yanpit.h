#ifndef RANKD_YANPIT_H
#define RANKD_YANPIT_H

#include "descriptors/yanpitPubSubTypes.h"

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

class YanpitSubListener : public DataReaderListener {
public:
    YanpitSubListener() {}
    ~YanpitSubListener() override {}
    void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override {

    }
    void on_data_available(DataReader* reader) override {

    }
    Yanpit::YanpitStatus _message;
};

class YanpitSubscriber {
public:
    YanpitSubscriber() : _participant(nullptr), _subscriber(nullptr), _reader(nullptr), _topic(nullptr) {}
    virtual ~YanpitSubscriber() {
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
    bool execute() {
        DomainParticipantQos participantQos;
        participantQos.name("Participant_subscriber");
        _participant = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);
        if (_participant == nullptr) {
            return false;
        }

        // Create the subscription topic.
        _topic = _participant->create_topic("YanpitStatusTopic", "YanpitStatus", TOPIC_QOS_DEFAULT);
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
    std::mutex* _mutex_pointer;
    int _thread_sampling_rate = 100;
    DomainParticipant* _participant;
    Subscriber* _subscriber;
    DataReader* _reader;
    Topic* _topic;
    YanpitSubListener _listener;
};

#endif //RANKD_YANPIT_H
