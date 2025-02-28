#include "structs/dispatchers/api.h"

std::string api_result_to_string(const ApiResult& result) {
    switch (result) {
        case ApiResult::OK:
            return "Ok";
        case ApiResult::FAIL:
            return "Fail";
    }
}

API *API::get_instance(const std::string& logger_name) {
#ifdef FROM_SIMUZILLA
    return new API(logger_name);
#else
    static API* instance = new API(logger_name);
    return instance;
#endif
}

#ifdef FROM_SIMUZILLA

UUIDv4 API::deliver_request(const std::string &json_admission_request, int priority, const std::vector<uint8_t> &target, const std::vector<uint8_t>& own_id, const IdentifierType& type) {
    _logger->info("[API] Delivering a message from API to simulated Rank process, to {}, requesting {}.", target.front(), json_admission_request);
    EAR* ear_message = build_message_from_arguments(json_admission_request, priority, target, type);
    _dispatcher->enqueue_item(std::make_tuple(ear_message, own_id, type));
    _dispatcher->mark_origin(ear_message->uuid(), 100+own_id.at(0));

    return ear_message->uuid();
}

UUIDv4
API::replenish_request(const UUIDv4 &uuid, const std::vector<uint8_t> &target, const std::vector<uint8_t> &own_id,
                       const IdentifierType &type) {
    _logger->info("[API] Delivering a replenishing message from API to simulated Rank process, to {}, requesting to remove UUID {}.", target.front(),
                  display(uuid));
    REP* rep_message = build_message_from_arguments(uuid, target, type);

    _dispatcher->enqueue_item(std::make_tuple(rep_message, own_id, type));
    _dispatcher->mark_origin(rep_message->uuid(), 100+own_id.at(0));

    return rep_message->uuid();
}

#endif

API *API::communicate_result(const ApiResult &code, const std::string &message, const UUIDv4 &uuid) {
    // TODO Build this option to run also in Simuzilla.
#ifdef FROM_SIMUZILLA

#else
    rapidjson::Document json;
    rapidjson::Value value;
    auto allocator = json.GetAllocator();

    // Add error code integer value as "code".
    value.SetInt(static_cast<int>(code));
    json.AddMember("code", value, allocator);

    // Add error code value as "error".
    value.SetString(api_result_to_string(code).c_str(), api_result_to_string(code).size());
    json.AddMember("error", value, allocator);

    // Add message as "message".
    value.SetString(message.c_str(), message.size());
    json.AddMember("message", value, allocator);

    // Add UUID as "uuid".
    value.SetString(display(uuid).c_str(), display(uuid).size());
    json.AddMember("uuid", value, allocator);

    // Write JSON output as string.
    rapidjson::StringBuffer string_buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(string_buffer);
    json.Accept(writer);
    std::string line(string_buffer.GetString(), string_buffer.GetSize());

    // Get PID for this UUID.
    int pid = _dispatcher->get_pid_from(uuid);

    // Open client FIFO, in /tmp/rank._origin.at(_uuid).fifo, previously created.
    char client[RANK_CLIENT_API_FIFO_LENGTH];
    snprintf(client, RANK_CLIENT_API_FIFO_LENGTH, RANK_CLIENT_API_FIFO_TEMPLATE, (long) pid);
    int client_fd = open(client, O_WRONLY);
    if (client_fd == -1) {
        // TODO Handle this error.
        return this;
    }

    // Write to the FIFO.
    if (write(client_fd, line.c_str(), line.size()) != line.size()) {
        // TODO Handle this error.
        return this;
    }

    // Close the FIFO.
    if (close(client_fd) == -1) {
        // TODO Handle this error.
        return this;
    }
#endif

    return this;
}

API *API::set_dispatcher(Dispatcher *dispatcher) {
    _dispatcher = dispatcher;

    return this;
}

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
#ifdef FROM_SIMUZILLA

#else
        // Read value from the FIFO.
        uint8_t byte;
        while (_server_fifo.read(reinterpret_cast<char*>(&byte), 1)) {
            tester.push_back(byte);
            if (found_delimiter(tester)) {
                tester.resize(tester.size() - RANK_FIFO_DELIMITER_LENGTH);
                bytestream = std::move(tester);
            }
        }

        // Deserialize bytestream, convert it to message, and deliver it to the Rx queue in dispatcher.
        AdmissionRequest admission_request = deserialize(bytestream);
        EAR* ear_message = build_message_from_admission_request(admission_request);
        _dispatcher->enqueue_item(std::make_tuple(ear_message, ear_message->listener(), admission_request.target_type));
        _dispatcher->mark_origin(ear_message->uuid(), admission_request.pid);
#endif

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

API::API(const std::string& logger_name) {
    // Configure logger.
    _logger = spdlog::get(logger_name);

#ifndef FROM_SIMUZILLA
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
#endif
}

#ifndef FROM_SIMUZILLA
EAR* API::build_message_from_admission_request(const AdmissionRequest &admission_request) {
    // Specify a header.
    Header header = Header(RANK_HEADER_VERSION, MessageType::EAR, UUIDv4());

    // Specify a priority.
    uint8_t priority = admission_request.priority;

    // Specify the listener.
    uint8_t listener_length = static_cast<uint8_t>(admission_request.target_type);
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener{};
    std::copy(admission_request.target.begin(), admission_request.target.end(), listener.begin());

    // Specify the list of requirements.
    std::vector<uint8_t> payload = marshall(admission_request.requirements);
    uint16_t payload_length = payload.size();

    // Create EAR message.
    EAR* ear_message = new EAR(header, priority, listener_length, listener, payload_length, payload);

    return ear_message;
}
#else

EAR *API::build_message_from_arguments(const std::string &json_admission_request, int priority,
                                       const std::vector<uint8_t> &target, const IdentifierType &type) {
    // Specify a header.
    Header header = Header(RANK_HEADER_VERSION, MessageType::EAR, generate_new_uuid());

    // Specify the listener.
    uint8_t listener_length = static_cast<uint8_t>(type);
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener{};
    std::copy(target.begin(), target.end(), listener.begin());

    // Specify the list of requirements.
    std::vector<uint8_t> payload = marshall(json_admission_request);
    uint16_t payload_length = payload.size();

    // Create EAR message.
    EAR* ear_message = new EAR(header, priority, listener_length, listener, payload_length, payload);

    return ear_message;
}

REP *API::build_message_from_arguments(const UUIDv4& uuid,
                                       const std::vector<uint8_t> &target, const IdentifierType &type) {
    // Specify a header.
    Header header = Header(RANK_HEADER_VERSION, MessageType::REP, uuid);

    // Specify the listener.
    uint8_t listener_length = static_cast<uint8_t>(type);
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> listener{};
    std::copy(target.begin(), target.end(), listener.begin());

    // Create EAR message.
    REP* rep_message = new REP(header, listener_length, listener);

    return rep_message;
}

#endif