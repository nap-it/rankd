#include <iostream>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

using namespace std;

int main() {
  // Variable declaration.
  struct snmp_session session;
  struct snmp_session* snmp_session;
  struct snmp_pdu* pdu;
  struct snmp_pdu* response;

  oid OID[MAX_OID_LEN];
  size_t OID_length = MAX_OID_LEN;

  struct variable_list* variables;
  int status;

  // Initialize the SNMP library.
  cout << "Preparing to start." << endl;
  init_snmp("rankd-time");
  snmp_sess_init(&session); // this sets up the default values in.
  session.version = SNMP_VERSION_1;
  session.peername = "localhost";
  session.community = (u_char*)"public";
  session.community_len = strlen(reinterpret_cast<const char *>(session.community));
  cout << "Starting SNMP session with host " << session.peername << endl;

  // Open the session.
  snmp_session = snmp_open(&session);
  if (!snmp_session) {
    snmp_perror("ACK");
    snmp_log(LOG_ERR, "Something happened\n");
    exit(2);
  }

  // Create a PDU for our request.
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  //read_objid("SOCE-DESIGN-3524::rxPreemptionStartFrame.0", OID, &OID_length);
  get_node("SOCE-DESIGN-3524-MIB::rxPreemptionStartFrame.0", OID, &OID_length);
  cout << "SOCE-DESIGN-3524-MIB::rxPreemptionStartFrame.0" << endl;
  snmp_add_null_var(pdu, OID, OID_length);
  status = snmp_synch_response(snmp_session, pdu, &response);
  if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
    cout << "Got a successful answer" << endl;
    for (variables = response->variables; variables != nullptr; variables = variables->next_variable) {
      cout << "Variable " << variables->val.integer << " is being accessed." << endl;
      //print_variable(variables->name, variables->name_length, variables);
    }
  } else {
    if (status == STAT_SUCCESS) {
      cout << "Error in packet" << endl << "Reason is " << snmp_errstring(response->errstat) << endl;
    } else {
      snmp_sess_perror("snmpget", snmp_session);
    }
  }

  if (response) {
    snmp_free_pdu(response);
  }
  snmp_close(snmp_session);

  return 0;
}