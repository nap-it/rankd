#ifndef RANK_PRELUDE_DISPATCHER_H
#define RANK_PRELUDE_DISPATCHER_H

#include <cstdint>
#include <mutex>
#include <queue>
#include <vector>

#include "spdlog/spdlog.h"

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
#ifdef FROM_SIMUZILLA
    void set_topology_and_current_address(std::function<const std::vector<int>*()> topology, unsigned int address);
    Dispatcher* borrow_simulation_receiver_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>()> function);
    Dispatcher* borrow_simulation_sender_function(std::function<void(uint8_t, std::vector<uint8_t>)> function);
#endif
    ~Dispatcher();
private:
    Dispatcher();
    Sender* _sender = Sender::get_instance();
#ifdef FROM_SIMUZILLA
    ReceiverSimulation* _receiver_simulation = ReceiverSimulation::get_instance();
    RawReceiverSimulation* _raw_receiver_simulation = RawReceiverSimulation::get_instance();
#else
    ReceiverL2* _receiver_l2 = ReceiverL2::get_instance();
    RawReceiverL2* _raw_receiver_l2 = RawReceiverL2::get_instance();
    ReceiverL3* _receiver_l3 = ReceiverL3::get_instance();
    ReceiverDDS* _receiver_dds = ReceiverDDS::get_instance();
#endif
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _received_messages = new std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>();
    std::mutex _received_messages_locker{};
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _sending_messages = new std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>();
    std::mutex _sending_messages_locker{};
#ifdef FROM_SIMUZILLA
    std::shared_ptr<spdlog::logger> _logger = spdlog::get("simuzilla-logger");
#else
    std::shared_ptr<spdlog::logger> _logger = spdlog::get("rank-logger");
#endif
};

#endif //RANK_PRELUDE_DISPATCHER_H
