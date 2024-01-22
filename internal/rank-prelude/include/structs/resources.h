#ifndef RANK_PRELUDE_RESOURCES_H
#define RANK_PRELUDE_RESOURCES_H

#include <algorithm>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

#include "constants.h"

#include "structs/current_capabilities.h"
#include "structs/identifier.h"
#include "structs/requesting_capabilities.h"
#include "structs/reservation.h"

class Resources {
public:
    // Instance handling.
    static Resources* get_instance();

    // Bid estimation.
    float estimate_bid(const RequestingCapabilities& capabilities) const;
    float estimate_bid(const Reservation& reservation) const;

    // Reservation handling.
    Reservation* available_for_performance(const Reservation& statement, uint8_t priority);
    Resources* replenish_reservation(Reservation* reservation);
    Resources* mark_reservation(Reservation* reservation);
    Resources* mark_pre_reservation(Reservation* reservation);
    std::list<Reservation> reservations() const;
    size_t reservations_size() const;

    // Threading control mechanisms.
    Resources* execute();
    Resources* stop();
    bool is_running() const;
    void operator()();

    // Destructor.
    ~Resources();

private:
    Resources();
    std::list<Reservation> _reservations;
    CurrentCapabilities _current_capabilities;
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
    std::mutex _reservations_mutex;
};

#endif  // RANK_PRELUDE_RESOURCES_H
