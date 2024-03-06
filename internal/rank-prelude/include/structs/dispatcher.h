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
    size_t receive_bytes();
    size_t send_bytes(const std::vector<uint8_t>& bytes);
    bool receiving_queue_is_empty() const;
    Message* dequeue_message();
    ~Dispatcher();
private:
    Dispatcher();
    Sender* _sender = Sender::get_instance();
    ReceiverSimulation* _receiver_simulation = ReceiverSimulation::get_instance();
    ReceiverL2* _receiver_l2 = ReceiverL2::get_instance();
    RawReceiverL2* _raw_receiver_l2 = RawReceiverL2::get_instance();
    ReceiverL3* _receiver_l3 = ReceiverL3::get_instance();
    ReceiverDDS* _receiver_dds = ReceiverDDS::get_instance();
    std::queue<Message*>* _received_messages = new std::queue<Message*>();
    std::mutex _received_messages_locker{};
};

#endif //RANK_PRELUDE_DISPATCHER_H
