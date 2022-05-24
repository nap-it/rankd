#ifndef RANKD_RELYUM_SERVICES_H
#define RANKD_RELYUM_SERVICES_H

class Services;

#include "relyum/timesync.h"

class Services {
public:
  static Services& get_instance() {
    static Services instance;
    return instance;
  }
  void operator=(const Services& services) = delete;
  void snap() {
    _ptp->snap();
  }
  PTP* ptp() const { return _ptp; }

private:
  Services() {
    _ptp = &PTP::get_instance();
    snap();
  };
  PTP* _ptp{};
};

#endif // RANKD_RELYUM_SERVICES_H
