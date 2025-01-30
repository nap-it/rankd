#include "structs/dispatchers/api.h"

API *API::get_instance() {
    static API* instance = new API();
    return instance;
}

//#ifdef FROM_SIMUZILLA

void API::deliver_request(const std::string &json_admission_request, int priority, const std::vector<uint8_t> &target) {

}

//#else

void API::get_message_from_fifo() {

}

//#endif

API *API::execute() {
    if (_running) {
        return this;
    }

    _running = true;
    _thread = std::thread(std::ref(*this));

    return this;
}

API *API::stop() {
    if (not _running) {
        return this;
    }

    _running = false;
    _thread.join();

    return this;
}

bool API::is_running() const {
    return _running;
}

void API::operator()() {
    std::vector<uint8_t> bytestream;
    std::vector<uint8_t> tester;

    while (_running) {
        // TODO Complete this code.
//#ifdef FROM_SIMUZILLA

//#else
        // Read value from the FIFO.
        uint8_t byte;
        while (_server_fifo.read(reinterpret_cast<char*>(&byte), 1)) {
            tester.push_back(byte);
            if (found_delimiter(tester)) {
                tester.resize(tester.size() - RANK_FIFO_DELIMITER_LENGTH);
                bytestream = std::move(tester);
            }
        }

        // TODO
//#endif
    }
}

API::API() {
    // Create the well-known FIFO in system, and open it for reading.
    umask(0);
    if (mkfifo(RANK_SERVER_API_FIFO_PATH, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        // TODO Handle this error.
    }
    _server_fifo_fd = open(RANK_SERVER_API_FIFO_PATH, O_RDONLY);
    _server_fifo = std::ifstream(RANK_SERVER_API_FIFO_PATH, std::ios::in | std::ios::binary);
    if (_server_fifo_fd == -1) {
        // TODO Handle this error.
    }
}