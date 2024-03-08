#ifndef RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H
#define RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "structs/message.h"
#include "utils/messaging.h"

class ReceiverSimulation {
public:
    static ReceiverSimulation* get_instance() {
        static ReceiverSimulation instance;
        return &instance;
    }
    ReceiverSimulation(const ReceiverSimulation&) = delete;
    //void enqueue_bytes(const std::vector<uint8_t>& bytes);
    void set_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* queue, std::mutex* mutex);
    ReceiverSimulation* execute();
    ReceiverSimulation* stop();
    bool is_running();
    void operator()();
private:
    ReceiverSimulation() = default;
    bool _running = false;
    std::thread _thread;
    std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>> _queue{};
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _message_deposit = nullptr;
    std::mutex* _message_deposit_mutex = nullptr;
};

#endif //RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H
