#ifndef RANK_PRELUDE_DISPATCHER_RECEIVER_L3_H
#define RANK_PRELUDE_DISPATCHER_RECEIVER_L3_H

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "structs/message.h"

class ReceiverL3 {
public:
    static ReceiverL3* get_instance() {
        static ReceiverL3 instance;
        return &instance;
    }
    ReceiverL3(const ReceiverL3&) = delete;
    void enqueue_bytes(const std::vector<uint8_t>& bytes);
    void set_queue(std::queue<Message*>* queue, std::mutex* mutex);
    ReceiverL3* execute();
    ReceiverL3* stop();
    bool is_running();
    void operator()();
private:
    ReceiverL3();
    bool _running = false;
    std::thread _thread;
    std::queue<std::vector<uint8_t>> _queue{};
    std::queue<Message*>* _message_deposit = nullptr;
    std::mutex* _message_deposit_mutex;
};

#endif //RANK_PRELUDE_DISPATCHER_RECEIVER_L3_H
