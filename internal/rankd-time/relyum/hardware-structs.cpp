#include "relyum/hardware-structs.h"

Bridge *Bridge::get_instance() {
  if (_instance == nullptr) {
    _instance = new Bridge();
  }

  return _instance;
}

Bridge::Bridge() = default;

void Bridge::snap_snmp() {
  // Open the SNMP session.
  auto session = open_snmp_session();
  if (!session) {
    // TODO Handle this error.
  }

  // Create the set of PDU requests.
  struct snmp_pdu* request;
  struct snmp_pdu* response;
  oid OID[MAX_OID_LEN];
  size_t OID_length = MAX_OID_LEN;

  // Create a request message.
  request = snmp_pdu_create(SNMP_MSG_GET);
  get_node(compose_target(PortSNMP::HALF_DUPLEX, port_no(0)->identifier()).c_str(), OID, &OID_length);
  snmp_add_null_var(request, OID, OID_length);
  int status = snmp_synch_response(session, request, &response);
  if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
    port_no(0)->is_half_duplex(response->variables->val.integer != 0);
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

  // TODO Other requests.

  // Close the session.
  snmp_close(session);
}