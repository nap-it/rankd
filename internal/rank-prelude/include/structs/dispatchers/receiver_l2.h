#ifndef RANK_PRELUDE_DISPATCHER_RECEIVER_L2_H
#define RANK_PRELUDE_DISPATCHER_RECEIVER_L2_H

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "structs/message.h"

class ReceiverL2 {
public:
    static ReceiverL2* get_instance() {
        static ReceiverL2 instance;
        return &instance;
    }
    ReceiverL2(const ReceiverL2&) = delete;
    void enqueue_bytes(const std::vector<uint8_t>& bytes);
    void set_queue(std::queue<Message*>* queue, std::mutex* mutex);
    ReceiverL2* execute();
    ReceiverL2* stop();
    bool is_running();
    void operator()();
private:
    ReceiverL2();
    bool _running = false;
    std::thread _thread;
    std::queue<std::vector<uint8_t>> _queue{};
    std::queue<Message*>* _message_deposit = nullptr;
    std::mutex* _message_deposit_mutex;
};

#endif //RANK_PRELUDE_DISPATCHER_RECEIVER_L2_H
