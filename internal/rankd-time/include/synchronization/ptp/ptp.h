#ifndef RANKD_TIME_SYNC_PTP_PTP_H
#define RANKD_TIME_SYNC_PTP_PTP_H

#include <cstring>
#include <map>
#include <string>
#include <vector>

#include <poll.h>
#include <signal.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

extern "C" {
#include "linuxptp/pmc.h"
#include "linuxptp/util.h"
};

#ifndef RELEASE_TARGET
#include <iostream>
#include <ifaddrs.h>
#endif

#define IOCTL_ETHTOOL_REQ                                                      \
  0x8946 // Ethtool family of IOCTL requests constant identifier.
#define HW_CAPABILITY_MASK                                                     \
  0b1111101 // Mask to get if device has PTP hardware capabilities.
#define SW_CAPABILITY_MASK                                                     \
  0b0101100 // Mask to get if device has PTP software capabilities.
#define HW_TX_TS_MODE_MASK                                                     \
  0b0000110 // Mask to get if device has needed HW TS TX modes.
#define HW_RX_FILTER_MASK 0b1100000000000000 // Mask for RX filters.

struct ethtool_command_context {
  const char *devname;
  int fd;
  struct ifreq ifr;
};

enum class PTPCapability { UNCAPABLE, SOFTWARE_ONLY, HARDWARE };

static struct stat buffer;

class PTP {
public:
  static PTP& get_instance() {
    static PTP instance;
    return instance;
  }
  void operator=(const PTP& ptp) = delete;
  void snap();
  [[nodiscard]] bool is_running() const { return _running; }
  [[nodiscard]] bool is_applying_on_system() const { return _applying_on_system; }
  [[nodiscard]] long master_offset() const { return _master_offset; }
  [[nodiscard]] long ingress_time() const { return _ingress_time; }
  [[nodiscard]] double cumulative_scaled_rate_offset() const {
    return _cumulative_scaled_rate_offset;
  }
  [[nodiscard]] int scaled_last_grand_master_phase_change() const {
    return _scaled_last_grand_master_phase_change;
  }
  [[nodiscard]] uint16_t grand_master_time_base_indicator() const {
    return _grand_master_time_base_indicator;
  }
  [[nodiscard]] uint16_t last_grand_master_phase_change_ns_msb() const {
    return _last_grand_master_phase_change_ns_msb;
  }
  [[nodiscard]] uint16_t last_grand_master_phase_change_ns_lsb() const {
    return _last_grand_master_phase_change_ns_lsb;
  }
  [[nodiscard]] uint16_t last_grand_master_phase_change_fractional_ns() const {
    return _last_grand_master_phase_change_fractional_ns;
  }
  [[nodiscard]] bool is_grand_master_present() const { return _grand_master_present; }
  [[nodiscard]] const std::string &grand_master_identity() const {
    return _grand_master_identity;
  }
  friend std::ostream &operator<<(std::ostream &os, const PTP &ptp) {
    os << "_running: " << ptp._running
       << " _applying_on_system: " << ptp._applying_on_system
       << " _master_offset: " << ptp._master_offset
       << " _ingress_time: " << ptp._ingress_time
       << " _cumulative_scaled_rate_offset: "
       << ptp._cumulative_scaled_rate_offset
       << " _scaled_last_grand_master_phase_change: "
       << ptp._scaled_last_grand_master_phase_change
       << " _grand_master_time_base_indicator: "
       << ptp._grand_master_time_base_indicator
       << " _last_grand_master_phase_change_ns_msb: "
       << ptp._last_grand_master_phase_change_ns_msb
       << " _last_grand_master_phase_change_ns_lsb: "
       << ptp._last_grand_master_phase_change_ns_lsb
       << " _last_grand_master_phase_change_fractional_ns: "
       << ptp._last_grand_master_phase_change_fractional_ns
       << " _grand_master_present: " << ptp._grand_master_present
       << " _grand_master_identity: " << ptp._grand_master_identity;
    return os;
  }

private:
  PTP() {
    snap();
  }
  bool _running = stat("/var/run/ptp4l", &buffer) == 0;
  bool _applying_on_system{};
  long _master_offset{};
  long _ingress_time{};
  double _cumulative_scaled_rate_offset{};
  int _scaled_last_grand_master_phase_change{};
  uint16_t _grand_master_time_base_indicator{};
  uint16_t _last_grand_master_phase_change_ns_msb{};
  uint16_t _last_grand_master_phase_change_ns_lsb{};
  uint16_t _last_grand_master_phase_change_fractional_ns{};
  bool _grand_master_present{};
  uint8_t _grand_master_clock_class{};
  uint8_t _grand_master_clock_accuracy{};
  uint16_t _grand_master_offset_scaled_log_variance{};
  uint16_t _grand_master_current_utc_offset{};
  bool _grand_master_leap_61{};
  bool _grand_master_leap_59{};
  bool _grand_master_current_utc_offset_valid{};
  bool _grand_master_ptp_timescale{};
  bool _grand_master_time_traceable{};
  bool _grand_master_frequency_traceable{};
  uint8_t _grand_master_time_source{};
  std::string _grand_master_identity;
  std::string _parent_port_identity;
  uint8_t _parent_stats;
  uint16_t _observed_parent_offset_scaled_log_variance;
  int32_t _observed_parent_clock_phase_change_rate;
  uint8_t _grandmaster_priority_1;
  uint8_t _grandmaster_priority_2;
  uint16_t _steps_removed;
  double _offset_from_master;
  double _mean_path_delay;
  uint16_t _clock_type;
  std::string _physical_layer_protocol;
  std::string _physical_address;
  uint16_t _protocol_address;
  std::string _manufacturer_id;
  std::string _product_description;
  std::string _revision_data;
  std::string _user_description;
  std::string _profile_id;
  std::string _port_identity;
  std::string _port_state;
  int8_t _log_min_delay_request_interval;
  int64_t _peer_mean_path_delay;
  int8_t _log_announce_interval;
  uint8_t _announce_receipt_timeout;
  int8_t _log_sync_interval;
  uint8_t _delay_mechanism;
  int8_t _log_min_peer_delay_request_interval;
  uint8_t _version_number;
};

std::map<std::string, std::vector<std::string>>
get_ptp_capabilities(const std::string &interface);
PTPCapability capability_level_in(
    const std::map<std::string, std::vector<std::string>> &information);
PTPCapability ptp_capability_type(const std::string &interface_name);

// Debug functions.
#ifndef RELEASE_TARGET
void debug_all_interfaces_ptp_capabilities();
#endif

#endif // RANKD_TIME_SYNC_PTP_PTP_H
