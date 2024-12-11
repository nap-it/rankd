#ifndef RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H
#define RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
#include <vector>

#include "spdlog/spdlog.h"

#include "structs/message.h"
#include "utils/messaging.h"

class ReceiverSimulation {
public:
    static ReceiverSimulation* get_instance(const std::string& logger_name) {
#ifdef FROM_SIMUZILLA
        return new ReceiverSimulation(logger_name);
#else
        static ReceiverSimulation instance = ReceiverSimulation(logger_name);
        return &instance;
#endif
    }
    ReceiverSimulation(const ReceiverSimulation&) = delete;
    void set_queue(std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>* queue, std::mutex* mutex, std::condition_variable* referee);
    void set_message_deposit_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* queue, std::mutex* mutex);
    ReceiverSimulation* execute();
    ReceiverSimulation* stop();
    bool is_running();
    void operator()();
private:
    explicit ReceiverSimulation(const std::string& logger_name);
    bool _running = false;
    std::thread _thread;
    std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>* _queue = nullptr;
    std::mutex* _queue_mutex = nullptr;
    std::condition_variable* _referee;
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _message_deposit = nullptr;
    std::mutex* _message_deposit_mutex = nullptr;
    std::shared_ptr<spdlog::logger> _logger;
};

class RawReceiverSimulation {
public:
    static RawReceiverSimulation* get_instance(const std::string& logger_name) {
#ifdef FROM_SIMUZILLA
        return new RawReceiverSimulation(logger_name);
#else
        static RawReceiverSimulation instance = RawReceiverSimulation(logger_name);
        return &instance;
#endif
    }
    std::tuple<std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>*, std::mutex*, std::condition_variable*> queue_access();
    RawReceiverSimulation* receive_control_borrowing_from(ReceiverSimulation* controller);
    RawReceiverSimulation* execute();
    RawReceiverSimulation* stop();
    bool is_running();
    void operator()();
#ifdef FROM_SIMUZILLA
    RawReceiverSimulation* borrow_receiver_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>(void)> function);
#endif
    ~RawReceiverSimulation();
private:
    explicit RawReceiverSimulation(const std::string& logger_name);
    std::function<std::pair<uint8_t, std::vector<uint8_t>>(void)> _simulated_recv;
    bool _running = false;
    std::thread _thread;
    std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>* _queue = new std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>();
    std::mutex* _queue_mutex = new std::mutex();
    std::condition_variable* _referee = new std::condition_variable();
    ReceiverSimulation* _receiver_controller = nullptr;
    std::shared_ptr<spdlog::logger> _logger;
};

#endif //RANK_PRELUDE_DISPATCHER_RECEIVER_SIMULATION_H
