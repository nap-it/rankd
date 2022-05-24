#include <iostream>

#include <string>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/utilities.h>
#include <net-snmp/net-snmp-includes.h>

#define NETSNMP_DS_APP_DONT_FIX_PDUS 0

#include "time-lib.h"

using namespace std;

int main() {

  auto bridge = Bridge::get_instance();

  cout << "Port 0 is enabled? " << (bridge.port_no(0)->is_enabled() ? "Yes" : "No") << endl;
  cout << "Port 1 is enabled? " << (bridge.port_no(1)->is_enabled() ? "Yes" : "No") << endl;
  cout << "Port 2 is enabled? " << (bridge.port_no(2)->is_enabled() ? "Yes" : "No") << endl;
  cout << "Port 3 is enabled? " << (bridge.port_no(3)->is_enabled() ? "Yes" : "No") << endl;
  cout << "Internal Port is enabled? " << (bridge.internal_port()->is_enabled() ? "Yes" : "No") << endl;

  cout << endl;

  cout << "gPTP: " << endl;
  cout << "Master offset: " << bridge.services()->ptp()->master_offset() << endl;
  cout << "Ingress Time: " << bridge.services()->ptp()->ingress_time() << endl;

//  // Variable declaration.
//  netsnmp_session session;
//  netsnmp_session*ss;
//  netsnmp_pdu* pdu;
//  netsnmp_pdu* response;
//
//  oid OID[MAX_OID_LEN];
//  size_t OID_length;
//
//  netsnmp_variable_list* vars;
//  int status;
//
//  SOCK_STARTUP;
//
//  // Initialize the SNMP library.
//  //cout << "Preparing to start." << endl;
//  init_snmp("snmpapp");
//  snmp_sess_init(&session); // this sets up the default values in.
//  session.version = SNMP_VERSION_1;
//  session.peername = "localhost";
//  session.community = (u_char*)"public";
//  session.community_len = strlen(reinterpret_cast<const char *>(session.community));
//  //cout << "Starting SNMP session with host " << session.peername << endl;
//
//  // Open the session.
//  ss = snmp_open(&session);
//  if (!ss) {
//    snmp_perror("ACK");
//    snmp_log(LOG_ERR, "Something happened\n");
//    exit(2);
//  }
//
//  // Create a PDU for our request.
//  pdu = snmp_pdu_create(SNMP_MSG_GET);
//  OID_length = MAX_OID_LEN;
//  //read_objid(".1.3.6.1.4.1.47561.3524.2.6.21.0", OID, &OID_length);
//  status = get_node("SOCE-DESIGN-3524-MIB::PORT-1.phySpeed.0", OID, &OID_length);
//  if (!status) {
//    snmp_perror("Bad OID");
//  }
//  cout << "OID has now a value of " << OID_length << endl;
//  //cout << "SOCE-DESIGN-3524-MIB::PORT-1.phySpeed.0" << endl;
//  snmp_add_null_var(pdu, OID, OID_length);
//
//  status = snmp_synch_response(ss, pdu, &response);
//  if (status == STAT_SUCCESS) {
//    if (response->errstat == SNMP_ERR_NOERROR) {
//      cout << "Got a successful answer" << endl;
//      cout << "Rid: " << response->reqid << endl;
//      cout << "Leng: " << response->variables->val_len << endl;
//      cout << "Request name: ";
//      for (int i = 0; i != OID_length; i++) {
//        cout << "." << OID[i];
//      }
//      cout << endl;
//      cout << "Response name: ";
//      for (int i = 0; i != 12; i++) {
//        cout << "." << response->variables->name[i];
//      }
//      cout << endl;
//      for (vars = response->variables; vars; vars = vars->next_variable) {
//        print_variable(vars->name, vars->name_length, vars);
//        cout << "Variable " << *vars->val.integer << " is being accessed."
//             << endl;
//
//      }
//    }
//  } else {
//    if (status == STAT_SUCCESS) {
//      cout << "Error in packet" << endl << "Reason is " << snmp_errstring(response->errstat) << endl;
//    } else {
//      snmp_sess_perror("snmpget", ss);
//    }
//  }
//
//  if (response) {
//    snmp_free_pdu(response);
//  }
//  snmp_close(ss);
//
//  SOCK_CLEANUP;

  return 0;
}