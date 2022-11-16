#include "configuration/srp/relyum/timesync.h"
#include "linuxptp/util.h"

void PTP::snap() {

#ifdef _SNMP_REQUEST_INFO
  auto session = open_snmp_session();
  if (!session) {
    // TODO Handle this error.
  }

  netsnmp_pdu *request;
  netsnmp_pdu *response;
  oid request_id[MAX_OID_LEN];
  size_t request_id_len = MAX_OID_LEN;

  // Create information request message.
  request = snmp_pdu_create(SNMP_MSG_GET);

  // Compose request for PTP info on board.
  compose_ptp_information_request(request, request_id, &request_id_len);

  int status = snmp_synch_response(session, request, &response);
  if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
    auto variable = response->variables;
    _running = *variable->val.integer != 0;
    variable = variable->next_variable;
    _applying_on_system = *variable->val.integer != 0;
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
  snmp_close(session);

#elif _RESTCONF_REQUEST_INFO
  // TODO Fill with RESTCONF code here...
#endif

  // Get configuration from PTP for linux.
  auto* pmc_configuration = config_create();
  if (!pmc_configuration) {
    // TODO Handle this error.
  }

  // Set flags to allow polling ptp4l.
  if (config_set_int(pmc_configuration, "network_transport", TRANS_UDS)) {
    // TODO Handle this error.
  }
  if (config_set_int(pmc_configuration, "transportSpecific", 1)) {
    // TODO Handle this error.
  }
  uint8_t boundary_hops = 0;

  auto transport_type = TRANS_UDS;
  auto transport_specific = config_get_int(pmc_configuration, nullptr, "transportSpecific") << 4;
  auto domain_number = config_get_int(pmc_configuration, nullptr, "domainNumber");
  auto interface_name = "/var/run/pmc." + std::to_string(getpid()); // TODO Could this lead to errors?

  auto pmc = pmc_create(pmc_configuration, transport_type, interface_name.c_str(), boundary_hops, domain_number, transport_specific, 0);
  if (!pmc) {
    // TODO Handle this error.
  }

  struct pollfd pollfd;
  pollfd.fd = pmc_get_transport_fd(pmc);
  pollfd.events = POLLIN | POLLPRI | POLLOUT;

  int count = poll(&pollfd, 1, 100);
  if (count < 0) {
    // TODO Handle this error.
  }

  if (pollfd.revents & POLLOUT) {
    if (pmc_do_command(pmc, std::string("GET TIME_STATUS_NP\n").data())) {
      // TODO Handle this error.
    }
  }

  if (pollfd.revents & (POLLIN|POLLPRI|POLLOUT)) {
    struct ptp_message* message = pmc_recv(pmc);
    if (message) {
      auto* time_status = (struct time_status_np*) ((struct management_tlv*) message->management.suffix)->data;
      _master_offset = time_status->master_offset;
      _ingress_time = time_status->ingress_time;
      _cumulative_scaled_rate_offset = (time_status->cumulativeScaledRateOffset + 0.0) / ((double)(1ULL << 41));
      _scaled_last_grand_master_phase_change = time_status->scaledLastGmPhaseChange;
      _grand_master_time_base_indicator = time_status->gmTimeBaseIndicator;
      _last_grand_master_phase_change_ns_msb = time_status->lastGmPhaseChange.nanoseconds_msb;
      _last_grand_master_phase_change_ns_lsb = time_status->lastGmPhaseChange.nanoseconds_lsb;
      _last_grand_master_phase_change_fractional_ns = time_status->lastGmPhaseChange.fractional_nanoseconds;
      _grand_master_present = time_status->gmPresent != 0;
      _grand_master_identity = cid2str(&time_status->gmIdentity);
    }
  }

  pmc_destroy(pmc);
  msg_cleanup();
  config_destroy(pmc_configuration);
}