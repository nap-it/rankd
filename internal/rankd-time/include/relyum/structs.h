#pragma once
#ifndef RANKD_RELYUM_HARDWARE_STRUCTS_H
#define RANKD_RELYUM_HARDWARE_STRUCTS_H

#include "relyum/snmp.h"

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
  void internal(bool internal) { _internal = internal; }
  void identifier(char identifier) { _identifier = identifier; }
  void enabled(bool enabled) { _enabled = enabled; }
  void phy_speed(PhySpeed phy_speed) { _phy_speed = phy_speed; }
  void rx_frames(long rx_frames) { _rx_frames = rx_frames; }
  void rx_dropped_overflowed_frame(long rx_dropped_overflowed_frame) {
    _rx_dropped_overflowed_frame = rx_dropped_overflowed_frame;
  }
  void rx_unicast_frames(long rx_unicast_frames) {
    _rx_unicast_frames = rx_unicast_frames;
  }
  void rx_multicast_frames(long rx_multicast_frames) {
    _rx_multicast_frames = rx_multicast_frames;
  }
  void rx_broadcast_frames(long rx_broadcast_frames) {
    _rx_broadcast_frames = rx_broadcast_frames;
  }
  void rx_vlan_tagged_frames(long rx_vlan_tagged_frames) {
    _rx_vlan_tagged_frames = rx_vlan_tagged_frames;
  }
  void rx_ptp_frames(long rx_ptp_frames) { _rx_ptp_frames = rx_ptp_frames; }
  void rx_overlength_frames(long rx_overlength_frames) {
    _rx_overlength_frames = rx_overlength_frames;
  }
  void rx_underlength_frames(long rx_underlength_frames) {
    _rx_underlength_frames = rx_underlength_frames;
  }
  void rx_preemption_start_frames(long rx_preemption_start_frames) {
    _rx_preemption_start_frames = rx_preemption_start_frames;
  }
  void rx_preemption_continue_frames(long rx_preemption_continue_frames) {
    _rx_preemption_continue_frames = rx_preemption_continue_frames;
  }
  void rx_preemption_bad_sequence_in_continue_frames(
      long rx_preemption_bad_sequence_in_continue_frames) {
    _rx_preemption_bad_sequence_in_continue_frames =
        rx_preemption_bad_sequence_in_continue_frames;
  }
  void
  rx_preemption_crc_error_in_frames(long rx_preemption_crc_error_in_frames) {
    _rx_preemption_crc_error_in_frames = rx_preemption_crc_error_in_frames;
  }
  void rx_bytes(long rx_bytes) { _rx_bytes = rx_bytes; }
  void crc_erroneous_frames(long crc_erroneous_frames) {
    _crc_erroneous_frames = crc_erroneous_frames;
  }
  void lan_id_erroneous_frames(long lan_id_erroneous_frames) {
    _lan_id_erroneous_frames = lan_id_erroneous_frames;
  }
  void tx_frames(long tx_frames) { _tx_frames = tx_frames; }
  void tx_dropped_overflowed_frames(long tx_dropped_overflowed_frames) {
    _tx_dropped_overflowed_frames = tx_dropped_overflowed_frames;
  }
  void tx_unicast_frames(long tx_unicast_frames) {
    _tx_unicast_frames = tx_unicast_frames;
  }
  void tx_multicast_frames(long tx_multicast_frames) {
    _tx_multicast_frames = tx_multicast_frames;
  }
  void tx_broadcast_frames(long tx_broadcast_frames) {
    _tx_broadcast_frames = tx_broadcast_frames;
  }
  void tx_vlan_tagged_frames(long tx_vlan_tagged_frames) {
    _tx_vlan_tagged_frames = tx_vlan_tagged_frames;
  }
  void tx_ptp_frames(long tx_ptp_frames) { _tx_ptp_frames = tx_ptp_frames; }
  void tx_bytes(long tx_bytes) { _tx_bytes = tx_bytes; }
  void tx_preemption_start_frames(long tx_preemption_start_frames) {
    _tx_preemption_start_frames = tx_preemption_start_frames;
  }
  void tx_preemption_continue_frames(long tx_preemption_continue_frames) {
    _tx_preemption_continue_frames = tx_preemption_continue_frames;
  }
  [[nodiscard]] bool is_internal() const { return _internal; }
  [[nodiscard]] bool is_enabled() const { return _enabled; }
  [[nodiscard]] PhySpeed phy_speed() const { return _phy_speed; }
  [[nodiscard]] long rx_frames() const { return _rx_frames; }
  [[nodiscard]] long rx_dropped_overflowed_frame() const {
    return _rx_dropped_overflowed_frame;
  }
  [[nodiscard]] long rx_unicast_frames() const { return _rx_unicast_frames; }
  [[nodiscard]] long rx_multicast_frames() const { return _rx_multicast_frames; }
  [[nodiscard]] long rx_broadcast_frames() const { return _rx_broadcast_frames; }
  [[nodiscard]] long rx_vlan_tagged_frames() const { return _rx_vlan_tagged_frames; }
  [[nodiscard]] long rx_ptp_frames() const { return _rx_ptp_frames; }
  [[nodiscard]] long rx_overlength_frames() const { return _rx_overlength_frames; }
  [[nodiscard]] long rx_underlength_frames() const { return _rx_underlength_frames; }
  [[nodiscard]] long rx_preemption_start_frames() const {
    return _rx_preemption_start_frames;
  }
  [[nodiscard]] long rx_preemption_continue_frames() const {
    return _rx_preemption_continue_frames;
  }
  [[nodiscard]] long rx_preemption_bad_sequence_in_continue_frames() const {
    return _rx_preemption_bad_sequence_in_continue_frames;
  }
  [[nodiscard]] long rx_preemption_crc_error_in_frames() const {
    return _rx_preemption_crc_error_in_frames;
  }
  [[nodiscard]] long rx_bytes() const { return _rx_bytes; }
  [[nodiscard]] long crc_erroneous_frames() const { return _crc_erroneous_frames; }
  [[nodiscard]] long lan_id_erroneous_frames() const { return _lan_id_erroneous_frames; }
  [[nodiscard]] long tx_frames() const { return _tx_frames; }
  [[nodiscard]] long tx_dropped_overflowed_frames() const {
    return _tx_dropped_overflowed_frames;
  }
  [[nodiscard]] long tx_unicast_frames() const { return _tx_unicast_frames; }
  [[nodiscard]] long tx_multicast_frames() const { return _tx_multicast_frames; }
  [[nodiscard]] long tx_broadcast_frames() const { return _tx_broadcast_frames; }
  [[nodiscard]] long tx_vlan_tagged_frames() const { return _tx_vlan_tagged_frames; }
  [[nodiscard]] long tx_ptp_frames() const { return _tx_ptp_frames; }
  [[nodiscard]] long tx_bytes() const { return _tx_bytes; }
  [[nodiscard]] long tx_preemption_start_frames() const {
    return _tx_preemption_start_frames;
  }
  [[nodiscard]] long tx_preemption_continue_frames() const {
    return _tx_preemption_continue_frames;
  }

private:
  bool _internal;
  char _identifier;
  bool _enabled;
  bool _is_half_duplex{};
  PhySpeed _phy_speed = PhySpeed::MBPS_1000;
  // Reception values (Rx values).
  long _rx_frames{};
  long _rx_dropped_overflowed_frame{};
  long _rx_unicast_frames{};
  long _rx_multicast_frames{};
  long _rx_broadcast_frames{};
  long _rx_vlan_tagged_frames{};
  long _rx_ptp_frames{};
  long _rx_overlength_frames{};
  long _rx_underlength_frames{};
  long _rx_preemption_start_frames{};
  long _rx_preemption_continue_frames{};
  long _rx_preemption_bad_sequence_in_continue_frames{};
  long _rx_preemption_crc_error_in_frames{};
  long _rx_bytes{};
  long _crc_erroneous_frames{};
  long _lan_id_erroneous_frames{};
  // Transmission values (Tx values).
  long _tx_frames{};
  long _tx_dropped_overflowed_frames{};
  long _tx_unicast_frames{};
  long _tx_multicast_frames{};
  long _tx_broadcast_frames{};
  long _tx_vlan_tagged_frames{};
  long _tx_ptp_frames{};
  long _tx_bytes{};
  long _tx_preemption_start_frames{};
  long _tx_preemption_continue_frames{};
};

class Bridge {
public:
  static Bridge& get_instance() {
    static Bridge instance;
    return instance;
  }
  void operator=(const Bridge& bridge) = delete;
  void snap_snmp();
  Port *port_no(char identifier = -1) {
    switch (identifier) {
    case 0:
      return &_port_0;
    case 1:
      return &_port_1;
    case 2:
      return &_port_2;
    case 3:
      return &_port_3;
    default:
      return &_internal_port;
    }
  }
  Port *internal_port() { return &_internal_port; }

private:
  Bridge() {
      snap_snmp();
  };
  Port _port_0 = Port(0);
  Port _port_1 = Port(1);
  Port _port_2 = Port(2);
  Port _port_3 = Port(3);
  Port _internal_port = Port();
  static Bridge *_instance;
};

#endif // RANKD_RELYUM_HARDWARE_STRUCTS_H
