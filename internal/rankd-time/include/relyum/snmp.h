#pragma once
#ifndef RANKD_RELYUM_SNMP_H
#define RANKD_RELYUM_SNMP_H

#include <string>


#include "net-snmp/net-snmp-config.h"
#include "net-snmp/net-snmp-includes.h"

const std::string MIB_PREFIX = "SOCE-DESIGN-3524-MIB::";

class Port;

#include "relyum/structs.h"

enum class PortSNMP {
  HALF_DUPLEX,
  PHY_SPEED,
  RX_FRAMES,
  TX_FRAMES,
  CRC_ERR_FRAMES,
  LANID_ERR_FRAMES,
  RX_DROP_OVERFLOW_FRAME,
  RX_UNICAST_FRAME,
  RX_MULTICAST_FRAME,
  RX_BROADCAST_FRAME,
  RX_VLAN_TAG_FRAME,
  RX_PTP_FRAME,
  RX_OVERLENGTH_FRAME,
  RX_DATA_BYTES,
  ENABLED_PORT,
  TX_DROP_OVERFLOW_FRAME,
  TX_UNICAST_FRAME,
  TX_MULTICAST_FRAME,
  TX_BROADCAST_FRAME,
  TX_VLAN_TAG_FRAME,
  TX_PTP_FRAME,
  TX_DATA_BYTES,
  RX_PREEMPTION_START_FRAME,
  RX_PREEMPTION_CONTINUE_FRAME,
  RX_PREEMPTION_BAD_SEQUENCE_CONTINUE_FRAME,
  RX_PREEMPTION_CRC_ERROR_FRAME,
  TX_PREEMPTION_START_FRAME,
  TX_PREEMPTION_CONTINUE_FRAME
};

std::string get_snmp_name(const PortSNMP& item);

std::string compose_target(const PortSNMP& item, char unit);

netsnmp_session* open_snmp_session();

int compose_request_port(char port, netsnmp_pdu* request, oid* OID, size_t* OID_length);

int fill_port_information(Port* port, netsnmp_pdu response);

#endif // RANKD_RELYUM_SNMP_H
