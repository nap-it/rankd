#ifndef RANK_PRELUDE_DISPATCHER_RECEIVER_DDS_H
#define RANK_PRELUDE_DISPATCHER_RECEIVER_DDS_H

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "structs/message.h"

class ReceiverDDS {
public:
    static ReceiverDDS* get_instance() {
        static ReceiverDDS instance;
        return &instance;
    }
    ReceiverDDS(const ReceiverDDS&) = delete;
    void enqueue_bytes(const std::vector<uint8_t>& bytes);
    void set_queue(std::queue<Message*>* queue, std::mutex* mutex);
    ReceiverDDS* execute();
    ReceiverDDS* stop();
    bool is_running();
    void operator()();
private:
    ReceiverDDS();
    bool _running = false;
    std::thread _thread;
    std::queue<std::vector<uint8_t>> _queue{};
    std::queue<Message*>* _message_deposit = nullptr;
    std::mutex* _message_deposit_mutex;
};

#endif //RANK_PRELUDE_DISPATCHER_RECEIVER_DDS_H
