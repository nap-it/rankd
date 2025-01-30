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

#include "constants.h"

//#ifndef FROM_SIMUZILLA
bool found_delimiter(const std::vector<uint8_t>& bytestream) {
    if (bytestream.size() != RANK_FIFO_DELIMITER.size()) {
        return false;
    }

    return std::equal(RANK_FIFO_DELIMITER.begin(), RANK_FIFO_DELIMITER.end(), bytestream.begin());
}
//#endif

class API {
public:
    static API* get_instance();

//#ifdef FROM_SIMUZILLA
    void deliver_request(const std::string& json_admission_request, int priority, const std::vector<uint8_t>& target);
//#else
    void get_message_from_fifo();
//#endif
    API* execute();
    API* stop();
    bool is_running() const;
    void operator()();
private:
    API();
//#ifndef FROM_SIMUZILLA
    std::ifstream _server_fifo;
    int _server_fifo_fd;
//#endif
    bool _running = false;
    std::thread _thread;
};

#endif //RANK_PRELUDE_DISPATCHER_API_H
