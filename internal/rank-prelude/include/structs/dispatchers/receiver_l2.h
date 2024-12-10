#ifndef RANK_PRELUDE_DISPATCHER_RECEIVER_L2_H
#define RANK_PRELUDE_DISPATCHER_RECEIVER_L2_H

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <unistd.h>
#include <net/if.h>
#include <linux/bpf.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "spdlog/spdlog.h"

#include "constants.h"
#include "structs/message.h"
#include "structs/dispatchers/bpf/rank_l2.skel.h"
#include "utils/messaging.h"

#define TEMPORARY_BUFFER_LENGTH 65536

class ReceiverL2 {
public:
    static ReceiverL2* get_instance(const std::string& logger_name) {
        static ReceiverL2 instance = ReceiverL2(logger_name);
        return &instance;
    }
    ReceiverL2(const ReceiverL2&) = delete;
    void set_queue(std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>* queue, std::mutex* mutex);
    void set_message_deposit_queue(std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* queue, std::mutex* mutex);
    ReceiverL2* execute();
    ReceiverL2* stop();
    bool is_running();
    void operator()();
private:
    ReceiverL2(const std::string& logger_name);
    bool _running = false;
    std::thread _thread;
    std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>* _queue = nullptr;
    std::mutex* _queue_mutex;
    std::queue<std::tuple<Message*, std::vector<uint8_t>, IdentifierType>>* _message_deposit = nullptr;
    std::mutex* _message_deposit_mutex = nullptr;
    std::shared_ptr<spdlog::logger> _logger;
};

class RawReceiverL2 {
public:
    static RawReceiverL2* get_instance(const std::string& logger_name) {
        static RawReceiverL2 instance = RawReceiverL2(logger_name);
        return &instance;
    }
    std::pair<std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>*, std::mutex*> queue_access();
    RawReceiverL2* receive_control_borrowing_from(ReceiverL2* controller);
    RawReceiverL2* execute();
    RawReceiverL2* stop();
    bool is_running();
    void operator()();
    ~RawReceiverL2();
private:
    RawReceiverL2(const std::string& logger_name);
    bool _running = false;
    int _raw_socket;
    unsigned char* _temporary_buffer;
    std::thread _thread;
    std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>* _queue = new std::queue<std::pair<std::vector<uint8_t>, std::vector<uint8_t>>>();
    std::mutex* _queue_mutex;
    ReceiverL2* _receiver_controller = nullptr;
    std::shared_ptr<spdlog::logger> _logger;
};


#endif //RANK_PRELUDE_DISPATCHER_RECEIVER_L2_H
