#ifndef RANK_PRELUDE_DISPATCHER_H
#define RANK_PRELUDE_DISPATCHER_H

#include <cstdint>
#include <mutex>
#include <queue>
#include <vector>

#include "structs/message.h"
#include "structs/dispatchers/all.h"

class Dispatcher {
public:
    static Dispatcher* get_instance() {
        static Dispatcher instance;
        return &instance;
    }
    void send_message(Message* message, const std::vector<uint8_t>& target, const IdentifierType& type);
    bool receiving_queue_is_empty() const;
    bool receiving_queue_has_message() const;
    std::tuple<Message*, std::vector<uint8_t>, IdentifierType> dequeue_item();
    ~Dispatcher();
private:
    Dispatcher();
    Sender* _sender = Sender::get_instance();
    ReceiverSimulation* _receiver_simulation = ReceiverSimulation::get_instance();
    ReceiverL2* _receiver_l2 = ReceiverL2::get_instance();
    RawReceiverL2* _raw_receiver_l2 = RawReceiverL2::get_instance();
    ReceiverL3* _receiver_l3 = ReceiverL3::get_instance();
    ReceiverDDS* _receiver_dds = ReceiverDDS::get_instance();
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _received_messages = new std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>();
    std::mutex _received_messages_locker{};
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _sending_messages = new std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>();
    std::mutex _sending_messages_locker{};
};

#endif //RANK_PRELUDE_DISPATCHER_H
