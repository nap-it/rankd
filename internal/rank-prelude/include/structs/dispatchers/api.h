#ifndef RANK_PRELUDE_DISPATCHER_API_H
#define RANK_PRELUDE_DISPATCHER_API_H

#include <fstream>
#include <string>
#include <thread>
#include <vector>

//#ifndef FROM_SIMUZILLA
#include <fcntl.h>
#include <sys/stat.h>
//#endif

#include "spdlog/spdlog.h"

class Dispatcher;

#include "constants.h"
#include "structs/dispatcher.h"
#include "structs/messages/ear.h"

#ifndef FROM_SIMUZILLA
static bool found_delimiter(const std::vector<uint8_t>& bytestream) {
    if (bytestream.size() != RANK_FIFO_DELIMITER.size()) {
        return false;
    }

    return std::equal(RANK_FIFO_DELIMITER.begin(), RANK_FIFO_DELIMITER.end(), bytestream.begin());
}
#endif

class API {
public:
    static API* get_instance(const std::string& logger_name);

#ifdef FROM_SIMUZILLA
    void deliver_request(const std::string& json_admission_request, int priority, const std::vector<uint8_t>& target, const IdentifierType& type);
#endif
    API* set_dispatcher(Dispatcher* dispatcher);
    API* execute();
    API* stop();
    bool is_running() const;
    void operator()();
private:
    explicit API(const std::string& logger_name);
#ifndef FROM_SIMUZILLA
    static EAR* build_message_from_admission_request(const AdmissionRequest& admission_request);
    std::ifstream _server_fifo;
    int _server_fifo_fd;
#else
    static EAR* build_message_from_arguments(const std::string& json_admission_request, int priority, const std::vector<uint8_t>& target, const IdentifierType& type);
#endif
    Dispatcher* _dispatcher;
    std::shared_ptr<spdlog::logger> _logger;
    bool _running = false;
    std::thread _thread;
};

#endif //RANK_PRELUDE_DISPATCHER_API_H
