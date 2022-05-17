#include "relyum/snmp.h"

std::string get_snmp_name(const PortSNMP& item) {
  switch (item) {
  case PortSNMP::HALF_DUPLEX:
    return "halfDuplex";
  case PortSNMP::PHY_SPEED:
    return "phySpeed";
  case PortSNMP::RX_FRAMES:
    return "rxFrames";
  case PortSNMP::TX_FRAMES:
    return "txFrames";
  case PortSNMP::CRC_ERR_FRAMES:
    return "crcErrFrames";
  case PortSNMP::LANID_ERR_FRAMES:
    return "lanidErrFrames";
  case PortSNMP::RX_DROP_OVERFLOW_FRAME:
    return "rxDropOverflowFrame";
  case PortSNMP::RX_UNICAST_FRAME:
    return "rxUnicastFrame";
  case PortSNMP::RX_MULTICAST_FRAME:
    return "rxMulticastFrame";
  case PortSNMP::RX_BROADCAST_FRAME:
    return "rxBroadcastFrame";
  case PortSNMP::RX_VLAN_TAG_FRAME:
    return "rxVlanTagFrame";
  case PortSNMP::RX_PTP_FRAME:
    return "rxPtpFrame";
  case PortSNMP::RX_OVERLENGTH_FRAME:
    return "rxOverlengthFrame";
  case PortSNMP::RX_DATA_BYTES:
    return "rxDataBytes";
  case PortSNMP::ENABLED_PORT:
    return "enablePort";
  case PortSNMP::TX_DROP_OVERFLOW_FRAME:
    return "txDropOverflowFrame";
  case PortSNMP::TX_UNICAST_FRAME:
    return "txUnicastFrame";
  case PortSNMP::TX_MULTICAST_FRAME:
    return "txMulticastFrame";
  case PortSNMP::TX_BROADCAST_FRAME:
    return "txBroadcastFrame";
  case PortSNMP::TX_VLAN_TAG_FRAME:
    return "txVlanTagFrame";
  case PortSNMP::TX_PTP_FRAME:
    return "txPtpFrame";
  case PortSNMP::TX_DATA_BYTES:
    return "txDataBytes";
  case PortSNMP::RX_PREEMPTION_START_FRAME:
    return "rxPreemptionStartFrame";
  case PortSNMP::RX_PREEMPTION_CONTINUE_FRAME:
    return "rxPreemptionContinueFrame";
  case PortSNMP::RX_PREEMPTION_BAD_SEQUENCE_CONTINUE_FRAME:
    return "rxPreemptionBadSequenceContinueFrame";
  case PortSNMP::RX_PREEMPTION_CRC_ERROR_FRAME:
    return "rxPreemptionCrcErrorFrame";
  case PortSNMP::TX_PREEMPTION_START_FRAME:
    return "txPreemptionStartFrame";
  case PortSNMP::TX_PREEMPTION_CONTINUE_FRAME:
    return "txPreemptionContinueFrame";
  }

  return "";
}

std::string compose_target(const PortSNMP& item, char unit) {
  if (unit >= 0 && unit <= 3) {
    return MIB_PREFIX + "PORT-" + unit + "." + get_snmp_name(item) + ".0";
  } else {
    return MIB_PREFIX + "INTERNAL-PORT." + get_snmp_name(item) + ".0";
  }
}

struct snmp_session* open_snmp_session() {
  struct snmp_session session;

  init_snmp("rankd-time");
  snmp_sess_init(&session);
  session.version = SNMP_VERSION_1;
  session.peername = "localhost";
  session.community = (u_char*)"public";
  session.community_len = strlen(reinterpret_cast<const char*>(session.community));

  return snmp_open(&session);
}

int compose_request_port(char port, netsnmp_pdu* request, oid* OID, size_t* OID_length) {
  get_node(compose_target(PortSNMP::HALF_DUPLEX, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::PHY_SPEED, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_FRAMES, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_FRAMES, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::CRC_ERR_FRAMES, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::LANID_ERR_FRAMES, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_DROP_OVERFLOW_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_UNICAST_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_MULTICAST_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_BROADCAST_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_VLAN_TAG_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_PTP_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_OVERLENGTH_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_DATA_BYTES, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::ENABLED_PORT, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_DROP_OVERFLOW_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_UNICAST_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_MULTICAST_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_BROADCAST_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_VLAN_TAG_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_PTP_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_DATA_BYTES, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_PREEMPTION_START_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_PREEMPTION_CONTINUE_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_PREEMPTION_BAD_SEQUENCE_CONTINUE_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::RX_PREEMPTION_CRC_ERROR_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_PREEMPTION_START_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);
  get_node(compose_target(PortSNMP::TX_PREEMPTION_CONTINUE_FRAME, port).c_str(), OID, OID_length);
  snmp_add_null_var(request, OID, *OID_length);

  return 0;
}

int fill_port_information(Port* port, netsnmp_pdu response) {
  auto* variable = response.variables;

  port->is_half_duplex(*variable->val.integer != 0);
  variable = variable->next_variable;
  port->phy_speed(PhySpeed(*variable->val.integer));
  variable = variable->next_variable;
  port->rx_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->tx_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->crc_erroneous_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->lan_id_erroneous_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_dropped_overflowed_frame(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_unicast_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_multicast_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_broadcast_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_vlan_tagged_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_ptp_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_overlength_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_bytes(*variable->val.integer);
  variable = variable->next_variable;
  port->enabled(*variable->val.integer != 0);
  variable = variable->next_variable;
  port->tx_dropped_overflowed_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->tx_unicast_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->tx_multicast_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->tx_broadcast_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->tx_vlan_tagged_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->tx_ptp_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->tx_bytes(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_preemption_start_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_preemption_continue_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_preemption_bad_sequence_in_continue_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->rx_preemption_crc_error_in_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->tx_preemption_start_frames(*variable->val.integer);
  variable = variable->next_variable;
  port->tx_preemption_continue_frames(*variable->val.integer);
  variable = variable->next_variable;

  return variable == nullptr;
}