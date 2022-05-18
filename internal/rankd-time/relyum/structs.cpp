#include "relyum/structs.h"

void Bridge::snap_snmp() {
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
      fill_port_information(port_no(port), *response);
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
}