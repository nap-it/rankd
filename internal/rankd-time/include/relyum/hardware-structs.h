#ifndef RANKD_RELYUM_HARDWARE_STRUCTS_H
#define RANKD_RELYUM_HARDWARE_STRUCTS_H

#include "snmp.h"

enum class PhySpeed {
  FIRST_INVALID = 0,
  MBPS_10 = 1,
  MBPS_100 = 2,
  MBPS_1000 = 3,
  SECOND_INVALID = 4
};

class Port {
public:
  Port() : _internal(true), _identifier(0){};
  explicit Port(char id) : _internal(false), _identifier(id) {}
  char identifier() const { return _identifier; }
  bool is_half_duplex() const { return _is_half_duplex; }
  void is_half_duplex(bool is) { _is_half_duplex = is; }

private:
  bool _internal;
  char _identifier;
  bool _enabled;
  bool _is_half_duplex{};
  PhySpeed _phy_speed = PhySpeed::MBPS_1000;
  // Reception values (Rx values).
  int _rx_frames{};
  int _rx_dropped_overflowed_frame{};
  int _rx_unicast_frames{};
  int _rx_multicast_frames{};
  int _rx_broadcast_frames{};
  int _rx_vlan_tagged_frames{};
  int _rx_ptp_frames{};
  int _rx_overlength_frames{};
  int _rx_underlength_frames{};
  int _rx_preemption_start_frames{};
  int _rx_preemption_continue_frames{};
  int _rx_preemption_bad_sequence_in_continue_frames{};
  int _rx_preemption_crc_error_in_frames{};
  int _rx_bytes{};
  int _crc_erroneous_frames{};
  int _lan_id_erroneous_frames{};
  // Transmission values (Tx values).
  int _tx_frames{};
  int _tx_dropped_overflowed_frames{};
  int _tx_unicast_frames{};
  int _tx_multicast_frames{};
  int _tx_broadcast_frames{};
  int _tx_vlan_tagged_frames{};
  int _tx_ptp_frames{};
  int _tx_bytes{};
  int _tx_preemption_start_frames{};
  int _tx_preemption_continue_frames{};
};

class Bridge {
public:
  Bridge* get_instance();
  void snap_snmp();
private:
  Bridge();
  Port* port_no(char identifier=-1) {
    switch (identifier) {
    case 0: return &_port_0;
    case 1: return &_port_1;
    case 2: return &_port_2;
    case 3: return &_port_3;
    default: return &_internal_port;
    }
  }
  Port* internal_port() {
    return &_internal_port;
  }
  Port _port_0 = Port(0);
  Port _port_1 = Port(1);
  Port _port_2 = Port(2);
  Port _port_3 = Port(3);
  Port _internal_port = Port();
  Bridge *_instance = nullptr;
};

#endif // RANKD_RELYUM_HARDWARE_STRUCTS_H
