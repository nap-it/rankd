#include "configuration/srp/relyum/structs.h"

#ifdef _SNMP_REQUEST_INFO
int Port::update_information(netsnmp_pdu response) {
  auto* variable = response.variables;
  
  is_half_duplex(*variable->val.integer != 0);
  variable = variable->next_variable;
  phy_speed(PhySpeed(*variable->val.integer));
  variable = variable->next_variable;
  rx_frames(*variable->val.integer);
  variable = variable->next_variable;
  tx_frames(*variable->val.integer);
  variable = variable->next_variable;
  crc_erroneous_frames(*variable->val.integer);
  variable = variable->next_variable;
  lan_id_erroneous_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_dropped_overflowed_frame(*variable->val.integer);
  variable = variable->next_variable;
  rx_unicast_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_multicast_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_broadcast_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_vlan_tagged_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_ptp_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_overlength_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_bytes(*variable->val.integer);
  variable = variable->next_variable;
  enabled(*variable->val.integer != 0);
  variable = variable->next_variable;
  tx_dropped_overflowed_frames(*variable->val.integer);
  variable = variable->next_variable;
  tx_unicast_frames(*variable->val.integer);
  variable = variable->next_variable;
  tx_multicast_frames(*variable->val.integer);
  variable = variable->next_variable;
  tx_broadcast_frames(*variable->val.integer);
  variable = variable->next_variable;
  tx_vlan_tagged_frames(*variable->val.integer);
  variable = variable->next_variable;
  tx_ptp_frames(*variable->val.integer);
  variable = variable->next_variable;
  tx_bytes(*variable->val.integer);
  variable = variable->next_variable;
  rx_preemption_start_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_preemption_continue_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_preemption_bad_sequence_in_continue_frames(*variable->val.integer);
  variable = variable->next_variable;
  rx_preemption_crc_error_in_frames(*variable->val.integer);
  variable = variable->next_variable;
  tx_preemption_start_frames(*variable->val.integer);
  variable = variable->next_variable;
  tx_preemption_continue_frames(*variable->val.integer);
  variable = variable->next_variable;

  return variable == nullptr;
}
#endif

void Bridge::snap() {

#ifdef _SNMP_REQUEST_INFO
  // Open the SNMP session.
  auto session = open_snmp_session();
  if (!session) {
    // TODO Handle this error.
  }

  // For each port and for the internal port (port identified as 4).
  for (char port = 0; port <= 4; port++) {
    // Create the set of PDU requests.
    struct snmp_pdu *request;
    struct snmp_pdu *response;
    oid OID[MAX_OID_LEN];
    size_t OID_length = MAX_OID_LEN;

    // Create a request message.
    request = snmp_pdu_create(SNMP_MSG_GET);

    // Get information on Port P.
    compose_request_port(port, request, OID, &OID_length);

    int status = snmp_synch_response(session, request, &response);
    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
      port_no(port)->update_information(*response);
    } else {
      if (status == STAT_SUCCESS) {
        // TODO Handle this error.
      } else {
        // TODO Handle this error.
      }
    }
    if (response) {
      snmp_free_pdu(response);
    }

  }

  // Close the session.
  snmp_close(session);
#elif _RESTCONF_REQUEST_INFO
  // TODO Fill with RESTCONF code here...
#endif
}

Services* Bridge::services() const { return _services; }
