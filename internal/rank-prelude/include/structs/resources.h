#ifndef RANK_PRELUDE_RESOURCES_H
#define RANK_PRELUDE_RESOURCES_H

#include <algorithm>
#include <cmath>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "spdlog/spdlog.h"

#include "constants.h"

#include "structs/current_capabilities.h"
#include "structs/identifier.h"
#include "structs/requesting_capabilities.h"
#include "structs/reservation.h"

class Resources {
public:
    // Instance handling.
#ifdef FROM_SIMUZILLA
    static Resources* get_instance(const std::function<const std::vector<std::pair<uint8_t, uint8_t>>*()>& topology, unsigned int own_address, const std::string& logger_name);
#else
    static Resources* get_instance(const std::string& logger_name);
#endif

    // Bid estimation.
    float estimate_bid(const RequestingCapabilities& capabilities) const;
    double estimate_bid(const Reservation& reservation);

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
#ifdef FROM_SIMUZILLA
    Resources(const std::function<const std::vector<std::pair<uint8_t, uint8_t>>*()>& topology, unsigned int own_address, const std::string& logger_name);
#else
    explicit Resources(const std::string& logger_name);
#endif
    double proximity_hops_assessment(const std::vector<uint8_t>& target, bool update = false);
    double proximity_rtt_assessment(const std::vector<uint8_t>& target, bool update = false);
    double proximity_pdv_assessment(const std::vector<uint8_t>& target, bool update = false);
    double proximity_pl_assessment(const std::vector<uint8_t>& target, bool update = false);
    double bare_metal_resource_assessment(const RequestingCapabilities& requirements) const;
    double current_resource_assessment(const RequestingCapabilities& requirements) const;
    double proximity_assessment(const std::array<uint8_t, 16>& target, uint8_t target_length);
    double hysteresis_assessment(const Reservation& reservation) const;
    std::map<std::string, std::tuple<long, double, double, double, double>> _proximity_metrics{};
    std::list<Reservation> _reservations;
    CurrentCapabilities* _current_capabilities;
    unsigned int _waiting_time = 1000;
    bool _running = false;
    std::thread _thread;
    std::mutex _reservations_mutex;
    std::shared_ptr<spdlog::logger> _logger;
};

#endif  // RANK_PRELUDE_RESOURCES_H
