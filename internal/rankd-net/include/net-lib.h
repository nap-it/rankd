#ifndef RANKD_NET_LIB_H
#define RANKD_NET_LIB_H

#include <atomic>
#include <thread>

// RapidJSON inclusions.
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"

#include "data-sources/yanpit.h"
#include "structs/yanpit-interfaces.h"

class YanpitNetwork {
public:
    static YanpitNetwork* get_instance() {
        static YanpitNetwork instance;
        return &instance;
    }
    YanpitNetwork(const YanpitNetwork&) = delete;
    void operator=(const YanpitNetwork&) = delete;
    void snap();
    [[nodiscard]] std::map<std::string, YanpitNetworkInterface> interfaces() const {
        return _interfaces;
    }
    [[nodiscard]] rapidjson::Document json() const;
    friend std::ostream& operator<<(std::ostream& os, const YanpitNetwork& network);
private:
    YanpitNetwork();
    std::map<std::string, YanpitNetworkInterface> _interfaces;
    YanpitConnection* _connection = YanpitConnection::get_instance();
};

#endif  // RANKD_NET_LIB_H
