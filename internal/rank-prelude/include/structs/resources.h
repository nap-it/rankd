#ifndef RANK_PRELUDE_RESOURCES_H
#define RANK_PRELUDE_RESOURCES_H

#include <mutex>
#include <set>
#include <thread>
#include <vector>

#include "structs/current_capabilities.h"
#include "structs/identifier.h"
#include "structs/requesting_capabilities.h"
#include "structs/reservation.h"

class Resources {
public:
    // Instance handling.
    static Resources* get_instance();

    // Bid estimation.
    float estimate_bid(const std::vector<RequestingCapabilities>& capabilities) const;
    float estimate_bid(const Reservation& reservation) const;

    // Reservation handling.
    Reservation* available_for_performance(const Reservation& statement);
    Resources* replenish_reservation(const UUIDv4& uuid);
    Resources* mark_reservation(const UUIDv4& uuid);
    std::set<Reservation> reservations() const;
    size_t reservations_size() const;

    // Threading control mechanisms.
    Resources* execute();
    Resources* stop();
    bool is_running() const;

protected:
    void operator()();

private:
    Resources();
    std::set<Reservation> _reservations;
    CurrentCapabilities _current_capabilities;
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
    std::mutex _reservations_mutex;
};


#endif  // RANK_PRELUDE_RESOURCES_H
