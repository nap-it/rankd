#ifndef RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H
#define RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "structs/message.h"

class ReceiverSimulation {
public:
    static ReceiverSimulation* get_instance() {
        static ReceiverSimulation instance;
        return &instance;
    }
    ReceiverSimulation(const ReceiverSimulation&) = delete;
    void enqueue_bytes(const std::vector<uint8_t>& bytes);
    void set_queue(std::queue<Message*>* queue, std::mutex* mutex);
    ReceiverSimulation* execute();
    ReceiverSimulation* stop();
    bool is_running();
    void operator()();
private:
    ReceiverSimulation() = default;
    bool _running = false;
    std::thread _thread;
    std::queue<std::vector<uint8_t>> _queue{};
    std::queue<Message*>* _message_deposit = nullptr;
    std::mutex* _message_deposit_mutex;
};

#endif //RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H