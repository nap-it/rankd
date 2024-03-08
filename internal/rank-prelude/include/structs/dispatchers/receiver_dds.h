#ifndef RANK_PRELUDE_DISPATCHER_RECEIVER_DDS_H
#define RANK_PRELUDE_DISPATCHER_RECEIVER_DDS_H

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "structs/message.h"
#include "utils/messaging.h"

class ReceiverDDS {
public:
    static ReceiverDDS* get_instance() {
        static ReceiverDDS instance;
        return &instance;
    }
    ReceiverDDS(const ReceiverDDS&) = delete;
    //void enqueue_bytes(const std::vector<uint8_t>& bytes);
    void set_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* queue, std::mutex* mutex);
    ReceiverDDS* execute();
    ReceiverDDS* stop();
    bool is_running();
    void operator()();
private:
    ReceiverDDS();
    bool _running = false;
    std::thread _thread;
    std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> _queue{};
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _message_deposit = nullptr;
    std::mutex* _message_deposit_mutex = nullptr;
};

#endif //RANK_PRELUDE_DISPATCHER_RECEIVER_DDS_H
