#ifndef RANKD_NET_LIB_H
#define RANKD_NET_LIB_H

#include <atomic>
#include <thread>

// RapidJSON inclusions.
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"

#include "data-sources/yanpit.h"
#include "structs/interfaces.h"

class Network {
public:
    static Network* get_instance() {
        static Network instance;
        return &instance;
    }
    Network(const Network&) = delete;
    void operator=(const Network&) = delete;
    void snap();
    [[nodiscard]] std::map<std::string, NetworkInterface> interfaces() const {
        return _interfaces;
    }
    [[nodiscard]] rapidjson::Document json() const;
    friend std::ostream& operator<<(std::ostream& os, const Network& network);
private:
    Network();
    std::map<std::string, NetworkInterface> _interfaces;
    YanpitConnection* _connection = YanpitConnection::get_instance();
};

#endif  // RANKD_NET_LIB_H
