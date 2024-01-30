#ifndef RANK_PRELUDE_PROCESS_H
#define RANK_PRELUDE_PROCESS_H

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class Handler;

#include "constants.h"
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

class Process {
public:
    // Instance handling.
    static Process* get_instance();

    // Store and handlers management.
    bool store(Handler* handler);
    void delete_handler(const UUIDv4& id);
    Handler* create_handler(const UUIDv4& id);
    Handler* resume_handler(const UUIDv4& id);
    Handler* suspend_handler(const UUIDv4& id);
    Handler* get_handler(const UUIDv4& id);
    HandlerState get_handler_state(const UUIDv4& id);
    bool is_uuid_in_store(const UUIDv4& uuid);

    // Receiving data parsing.
    Header parse_as_message_header(const std::vector<uint8_t>& data);
    MessageType parse_as_message_type(const std::vector<uint8_t>& data);
    UUIDv4 parse_as_message_uuid(const std::vector<uint8_t>& data);

    // Threading control mechanisms.
    Process* execute();
    Process* stop();
    bool is_running() const;
    void operator()();

    // Destructor.
    ~Process();

private:
    Process();
    Resources* _resources;
    TimeoutHandler* _timeout_handler;
    Store _store;
    std::mutex _store_locker;
    TranslationTable _translation_table;
    std::mutex _translation_table_locker;
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
    int _socket;
    int _socket_options = 1;
    struct sockaddr_in _address{};
    socklen_t _address_length = sizeof(_address);
};


#endif  // RANK_PRELUDE_PROCESS_H
