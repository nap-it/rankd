#ifndef RANK_PRELUDE_DISPATCHER_SENDER_H
#define RANK_PRELUDE_DISPATCHER_SENDER_H

#include <mutex>
#include <queue>
#include <thread>
#include <tuple>

#include "structs/message.h"

#include "net-lib.h"

#include "EthLayer.h"
#include "PayloadLayer.h"
#include "Packet.h"
#include "PcapLiveDevice.h"
#include "PcapLiveDeviceList.h"

class Sender {
public:
    static Sender* get_instance() {
        static Sender instance;
        return &instance;
    }
    Sender* set_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* queue, std::mutex* mutex);
    Sender* execute();
    Sender* stop();
    bool is_running();
    void operator()();
private:
    Sender();
    void make_and_send_frame(Message* message, const std::vector<uint8_t>& target);
    void make_and_send_packet(const Message* message, const std::vector<uint8_t>& target) const;
    void make_and_send_message(const Message* message, const std::vector<uint8_t>& target) const;
    void make_and_send_bytes(const Message* message, const std::vector<uint8_t>& target) const;
    bool _running = false;
    std::thread _thread;
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _queue;
    std::mutex* _queue_mutex;
    NetworkNeighbors _neighbors_data_source = NetworkNeighbors();
};

#endif //RANK_PRELUDE_DISPATCHER_SENDER_H
