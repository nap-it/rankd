#ifndef RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H
#define RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "spdlog/spdlog.h"

#include "structs/message.h"
#include "utils/messaging.h"

class ReceiverSimulation {
public:
    static ReceiverSimulation* get_instance() {
        static ReceiverSimulation instance;
        return &instance;
    }
    ReceiverSimulation(const ReceiverSimulation&) = delete;
    void set_queue(std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>* queue, std::mutex* mutex);
    void set_message_deposit_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* queue, std::mutex* mutex);
    ReceiverSimulation* execute();
    ReceiverSimulation* stop();
    bool is_running();
    void operator()();
private:
    ReceiverSimulation() = default;
    bool _running = false;
    std::thread _thread;
    std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>* _queue = nullptr;
    std::mutex* _queue_mutex;
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _message_deposit = nullptr;
    std::mutex* _message_deposit_mutex = nullptr;
    std::shared_ptr<spdlog::logger> _logger = spdlog::get("rank-logger");
};

class RawReceiverSimulation {
public:
    static RawReceiverSimulation* get_instance() {
        static RawReceiverSimulation instance;
        return &instance;
    }
    std::pair<std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>*, std::mutex*> queue_access();
    RawReceiverSimulation* receive_control_borrowing_from(ReceiverSimulation* controller);
    RawReceiverSimulation* execute();
    RawReceiverSimulation* stop();
    bool is_running();
    void operator()();
#ifdef FROM_SIMUZILLA
    RawReceiverSimulation* borrow_receiver_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>(void)>* function);
#endif
    ~RawReceiverSimulation();
private:
    RawReceiverSimulation();
    void* _simulated_recv = nullptr;
    bool _running = false;
    std::thread _thread;
    std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>* _queue = new std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>();
    std::mutex* _queue_mutex;
    ReceiverSimulation* _receiver_controller = nullptr;
    std::shared_ptr<spdlog::logger> _logger = spdlog::get("rank-logger");
};

#endif //RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H
