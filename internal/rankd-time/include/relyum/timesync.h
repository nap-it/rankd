#ifndef RANKD_RELYUM_TIMESYNC_H
#define RANKD_RELYUM_TIMESYNC_H

#include <string>

#include <poll.h>
#include <unistd.h>

extern "C" {
#include "linuxptp/pmc.h"
};

class PTP;

#ifdef _SNMP_REQUEST_INFO
#include "relyum/snmp.h"
#elif _RESTCONF_REQUEST_INFO
// TODO Fill with RESTCONF code here...
#endif

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

private:
  PTP() {
    snap();
  }
  bool _running{};
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
  std::string _grand_master_identity;
};

#endif // RANKD_RELYUM_TIMESYNC_H
