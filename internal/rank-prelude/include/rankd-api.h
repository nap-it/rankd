#ifndef RANKD_API_H
#define RANKD_API_H

#define RANK_SERVER_API_FIFO_PATH "/tmp/rank.fifo"
#define RANK_CLIENT_API_FIFO_TEMPLATE "/tmp/rank.%ld.fifo"
#define RANK_CLIENT_API_FIFO_LENGTH (sizeof(RANK_CLIENT_API_FIFO_TEMPLATE) + 20)
#define RANK_FIFO_DELIMITER_LENGTH 6

#include <array>
#include <string>
#include <sstream>
#include <vector>
#include <sys/stat.h>

constexpr std::array<uint8_t, RANK_FIFO_DELIMITER_LENGTH> RANK_FIFO_DELIMITER = {0x23, 0x52, 0x41, 0x4e, 0x4b, 0x23};

struct AdmissionRequest {
    std::string requirements;
    uint8_t priority = 0;
    std::vector<uint8_t> target;
    uint32_t pid = 0;
};

inline AdmissionRequest deserialize(const std::vector<uint8_t>& serialized) {
    struct AdmissionRequest admission_request;

    // Create seek pointer to serialized content and forward it.
    int pointer = 0;

    // Read requirements length size and content.
    uint32_t requirements_length = 0;
    for (int i = 3; i >= 0; i--) {
        requirements_length |= (serialized[pointer++]) << i;
    }
    std::stringstream json_requirements_list;
    for (int i = 0; i != requirements_length; i++) {
        json_requirements_list << serialized[pointer++];
    }
    admission_request.requirements = json_requirements_list.str();

    // Read priority.
    admission_request.priority = serialized[pointer++];

    // Read target length size and content.
    uint8_t target_length = serialized[pointer++];
    std::vector<uint8_t> target;
    for (int i = 0; i != target_length; i++) {
        target.push_back(serialized[pointer++]);
    }
    admission_request.target = target;

    // Read PID from serialized data.
    uint32_t pid = 0;
    for (int i = 3; i >= 0; i--) {
        pid |= (serialized[pointer++]) << i;
    }
    admission_request.pid = pid;

    return admission_request;
}

inline std::vector<uint8_t> serialize_to_rank(const std::string& json_requirements_list, u_int8_t priority, const std::vector<u_int8_t>& target, uint32_t pid) {
    std::vector<uint8_t> serialized;

    // Get length of JSON requirements list and serialize both (length and value).
    uint32_t requirements_length = json_requirements_list.size();
    for (int i = 3; i >= 0; i--) {
        serialized.push_back((requirements_length >> i) & 0xFF);
    }
    for (const auto& byte : json_requirements_list) {
        serialized.push_back(byte);
    }

    // Get priority and serialize it.
    serialized.push_back(priority);

    // Get target length and add its value to serialized (length and value).
    serialized.push_back(target.size());
    for (const auto& byte : target) {
        serialized.push_back(byte);
    }

    // Get PID and set it serialized.
    for (int i = 3; i >= 0; i--) {
        serialized.push_back((pid >> i) & 0xFF);
    }

    return serialized;
}

inline std::string place_admission_request(const std::string& json_requirements_list, u_int8_t priority, const std::vector<u_int8_t>& target, uint32_t pid) {
    // Serialize input parameters.
    auto serialized = serialize_to_rank(json_requirements_list, priority, target, pid);

    // Create this side's client FIFO for Rank message.
    umask(0);
    char client[RANK_CLIENT_API_FIFO_LENGTH];
    snprintf(client, RANK_CLIENT_API_FIFO_LENGTH, RANK_CLIENT_API_FIFO_TEMPLATE, (long) getpid());
    if (mkfifo(client, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        // TODO Handle this error.
    }

    // Add delimiter to serialized data.
    for (const auto& byte : RANK_FIFO_DELIMITER) {
        serialized.push_back(byte);
    }

    // Place serialized message into Rank's server FIFO.
    int server_fifo = open(RANK_SERVER_API_FIFO_PATH, O_WRONLY);
    if (server_fifo == -1) {
        // TODO Handle this error.
    }
    if (write(server_fifo, serialized.data(), serialized.size()) != serialized.size()) {
        // TODO Handle this error.
    }

    return {client}; // TODO Who uses this, should terminate client fifo at exit (atexit(remove_fifo)).
}

#endif //RANKD_API_H
