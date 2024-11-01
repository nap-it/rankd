#ifndef RANKDTIME_SYNCHRONIZATION_H
#define RANKDTIME_SYNCHRONIZATION_H

#include <ostream>

#include "document.h"
#include "stringbuffer.h"
#include "writer.h"

#ifndef LINUX_PTP
#include <ptpmgmt/init.h>
#endif

#define OUTPUT_SYNC_TYPE_PTP 1
#define OUTPUT_SYNC_TYPE_ALL OUTPUT_SYNC_TYPE_PTP

class Synchronization {
public:
    Synchronization();
    void snap();
    void enable_json_output();
    void disable_json_output();
    void set_output(uint8_t type);
    [[nodiscard]] rapidjson::Document json() const;
    //friend std::string print_synchronization(const Synchronization& synchronization);
    //friend std::ostream& operator<<(std::ostream& os, const Synchronization& synchronization);
private:
#ifndef LINUX_PTP
    void snap_ptp_via_linux_ptpmgmt();
    //void snap_ptp_via_linux_ptp();
#elif define(RELYUM)
    void snap_ptp_via_relyum();
#endif
    bool _json_formatted_output = false;
    uint8_t _output_type;
};

#endif //RANKDTIME_SYNCHRONIZATION_H