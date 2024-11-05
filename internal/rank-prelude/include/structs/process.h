#ifndef RANK_PRELUDE_PROCESS_H
#define RANK_PRELUDE_PROCESS_H

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "spdlog/spdlog.h"

#define NLMSG_BUF_SIZE 4096

class Handler;

#include "constants.h"
#include "structs/dispatcher.h"
#include "structs/store.h"
#include "structs/timeout_table.h"
#include "structs/translation_table.h"
#include "structs/handler.h"
#include "structs/handler_state.h"
#include "structs/identifier.h"
#include "structs/messages/header.h"
#include "structs/messages/message_type.h"
#include "structs/resources.h"
#include "structs/timeout_handler.h"

#include "utils/messaging.h"

namespace rank {

class Process {
public:
    // Instance handling.
    static Process *get_instance();

    // Store and handlers management.
    bool store(Handler *handler);

    void delete_handler(const UUIDv4 &id);

    Handler *create_handler(const UUIDv4 &id);

    Handler *resume_handler(const UUIDv4 &id);

    Handler *suspend_handler(const UUIDv4 &id);

    Handler *get_handler(const UUIDv4 &id);

    HandlerState get_handler_state(const UUIDv4 &id);

    bool is_uuid_in_store(const UUIDv4 &uuid);

    // Receiving data parsing.
    Header parse_as_message_header(const std::vector<uint8_t> &data);

    MessageType parse_as_message_type(const std::vector<uint8_t> &data);

    UUIDv4 parse_as_message_uuid(const std::vector<uint8_t> &data);

    // Threading control mechanisms.
    Process *execute();

    Process *stop();

    bool is_running() const;

    void operator()();

    // Simulation set parameters.
#ifdef FROM_SIMUZILLA
    Process* set_topology_and_current_address(std::function<const std::vector<int>*()> topology, unsigned int address);

    Process *borrow_simulation_recv_function(std::function<std::pair<uint8_t, std::vector<uint8_t>>(void)> function);

    Process *borrow_simulation_send_function(std::function<void(uint8_t, std::vector<uint8_t>)> function);
#endif

    // Logging registry.
    Process* log_on(std::shared_ptr<spdlog::logger> logger);

    // Destructor.
    ~Process();

private:
    Process();

    Dispatcher *_dispatcher;
    Resources *_resources;
    TimeoutHandler *_timeout_handler;
    Store _store;
    std::mutex _store_locker;
    TranslationTable _translation_table;
    std::mutex _translation_table_locker;
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
    bool _in_simulation = false;
#ifdef FROM_SIMUZILLA
    std::shared_ptr<spdlog::logger> _logger = spdlog::get("simuzilla-logger");
#else
    std::shared_ptr<spdlog::logger> _logger = spdlog::get("rank-logger");
#endif
};

}


#endif  // RANK_PRELUDE_PROCESS_H
