#ifndef RANK_PRELUDE_DISPATCHER_SENDER_H
#define RANK_PRELUDE_DISPATCHER_SENDER_H

#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>

#include "spdlog/spdlog.h"

#include "structs/message.h"

#ifdef FROM_SIMUZILLA
#include "dry-net-lib.h"
#else
#include "net-lib.h"
#endif

#include "EthLayer.h"
#include "PayloadLayer.h"
#include "Packet.h"
#include "PcapLiveDevice.h"
#include "PcapLiveDeviceList.h"

class Sender {
public:
    static Sender* get_instance(const std::string& logger_name) {
#ifdef FROM_SIMUZILLA
        return new Sender(logger_name);
#else
        static Sender instance = Sender(logger_name);
        return &instance;
#endif
    }
    Sender* set_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* queue, std::mutex* mutex);
    Sender* execute();
    Sender* stop();
    bool is_running();
    void operator()();
#ifdef FROM_SIMUZILLA
    void set_topology_and_current_address(std::function<const std::vector<int>*()> topology, unsigned int address);
    Sender* borrow_sender_function(std::function<void(uint8_t, std::vector<uint8_t>)> function);
#endif
private:
    Sender(const std::string& logger_name);
    void make_and_send_frame(Message* message, const std::vector<uint8_t>& target);
    void make_and_send_packet(Message* message, const std::vector<uint8_t>& target) const;
    void make_and_send_message(Message* message, const std::vector<uint8_t>& target) const;
    void make_and_send_bytes(Message* message, const std::vector<uint8_t>& target) const;
    std::function<void(uint8_t, std::vector<uint8_t>)> _simulated_send;
    bool _running = false;
    std::thread _thread;
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _queue;
    std::mutex* _queue_mutex;
    std::shared_ptr<spdlog::logger> _logger;
#ifdef FROM_SIMUZILLA
    std::function<const std::vector<int>*()> _topology;
    unsigned int _own_address;
    NetworkNeighbors _neighbors_data_source = NetworkNeighbors(_topology, _own_address);
#else
    NetworkNeighbors _neighbors_data_source = NetworkNeighbors();
#endif
};

#endif //RANK_PRELUDE_DISPATCHER_SENDER_H
