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
    static Dispatcher *get_instance(const std::string &logger_name) {
#ifdef FROM_SIMUZILLA
        return new Dispatcher(logger_name);
#else
        static Dispatcher instance = Dispatcher(logger_name);
        return &instance;
#endif
    }

    Dispatcher *execute_dispatchers();

    Dispatcher *stop_dispatchers();

    void send_message(Message *message, const std::vector<uint8_t> &target, const IdentifierType &type);

    bool receiving_queue_is_empty() const;

    bool receiving_queue_has_message() const;

    std::tuple<Message *, std::vector<uint8_t>, IdentifierType> dequeue_item();

#ifdef FROM_SIMUZILLA

    void set_topology_and_current_address(std::function<const std::vector<int> *()> topology, unsigned int address);

    Dispatcher *borrow_simulation_receiver_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>()> function);

    Dispatcher *borrow_simulation_sender_function(std::function<void(uint8_t, std::vector<uint8_t>)> function);

#endif

    ~Dispatcher();

private:
    Dispatcher();

    explicit Dispatcher(const std::string &logger_name);

    Sender *_sender;
#ifdef FROM_SIMUZILLA
    ReceiverSimulation *_receiver_simulation = nullptr;
    RawReceiverSimulation *_raw_receiver_simulation = nullptr;
#else
    ReceiverL2* _receiver_l2 = nullptr;
    RawReceiverL2* _raw_receiver_l2 = nullptr;
    ReceiverL3* _receiver_l3 = nullptr;
    ReceiverDDS* _receiver_dds = nullptr;
#endif
    std::queue<std::tuple<Message *, std::vector<uint8_t>, IdentifierType>> *_received_messages = new std::queue<std::tuple<Message *, std::vector<uint8_t>, IdentifierType>>();
    std::mutex _received_messages_locker{};
    std::queue<std::tuple<Message *, std::vector<uint8_t>, IdentifierType>> *_sending_messages = new std::queue<std::tuple<Message *, std::vector<uint8_t>, IdentifierType>>();
    std::mutex _sending_messages_locker{};
    std::shared_ptr<spdlog::logger> _logger;
};
#endif //RANK_PRELUDE_DISPATCHER_H
