#ifndef RANKD_TIME_LAT_TAS_H
#define RANKD_TIME_LAT_TAS_H

#include <cstdint>
#include <cstring>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <linux/rtnetlink.h>

class TASEntry {
public:
    explicit TASEntry(const struct rtattr* attribute);
    static bool check_identity(const struct rtattr* attribute) {
        return attribute != nullptr && strcmp(static_cast<const char*>(RTA_DATA(attribute)), "taprio") == 0;
    }

private:
    uint8_t _number_of_traffic_classes;
    std::vector<uint8_t> _map{};
    std::vector<std::pair<uint8_t, uint8_t>> _queues{};
    std::optional<int32_t> _clock_id;
    std::string _clock_id_string{};
    std::optional<uint32_t> _flags;
    std::optional<uint32_t> _txtime_delay;
    int64_t _base_time = 0;
    int64_t _cycle_time = 0;
    int64_t _cycle_time_extension = 0;
    std::vector<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>> _schedule{};
};

class TAS {
public:

private:
    std::set<TASEntry> _entries;
};

#endif //RANKD_TIME_LAT_TAS_H
