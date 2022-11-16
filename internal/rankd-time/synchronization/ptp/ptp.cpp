#include "synchronization/ptp/ptp.h"

#include <iostream>

[[maybe_unused]] static std::vector<std::string> timestamping_labels = {
    "hardware-transmit",        // SOF_TIMESTAMPING_TX_HARDWARE
    "software-transmit",        // SOF_TIMESTAMPING_TX_SOFTWARE
    "hardware-receive",         // SOF_TIMESTAMPING_RX_HARDWARE
    "software-receive",         // SOF_TIMESTAMPING_RX_SOFTWARE
    "software-system-clock",    // SOF_TIMESTAMPING_SOFTWARE
    "hardware-legacy-clock",    // SOF_TIMESTAMPING_SYS_HARDWARE
    "hardware-raw-clock",       // SOF_TIMESTAMPING_RAW_HARDWARE
};

[[maybe_unused]] static std::vector<std::string> transmission_types = {
    "off",                      // HWTSTAMP_TX_OFF
    "on",                       // HWTSTAMP_TX_ON
    "one-step-sync",            // HWTSTAMP_TX_ONESTEP_SYNC
};

const std::vector<std::string> reception_filters = {
    "none",                     // HWTSTAMP_FILTER_NONE
    "all",                      // HWTSTAMP_FILTER_ALL
    "some",                     // HWTSTAMP_FILTER_SOME
    "ptpv1-l4-event",           // HWTSTAMP_FILTER_PTP_V1_L4_EVENT
    "ptpv1-l4-sync",            // HWTSTAMP_FILTER_PTP_V1_L4_SYNC
    "ptpv1-l4-delay-req",       // HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ
    "ptpv2-l4-event",           // HWTSTAMP_FILTER_PTP_V2_L4_EVENT
    "ptpv2-l4-sync",            // HWTSTAMP_FILTER_PTP_V2_L4_SYNC
    "ptpv2-l4-delay-req",       // HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ
    "ptpv2-l2-event",           // HWTSTAMP_FILTER_PTP_V2_L2_EVENT
    "ptpv2-l2-sync",            // HWTSTAMP_FILTER_PTP_V2_L2_SYNC
    "ptpv2-l2-delay-req",       // HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ
    "ptpv2-event",              // HWTSTAMP_FILTER_PTP_V2_EVENT
    "ptpv2-sync",               // HWTSTAMP_FILTER_PTP_V2_SYNC
    "ptpv2-delay-req",          // HWTSTAMP_FILTER_PTP_V2_DELAY_REQ
    "ntp-all",                  // HWTSTAMP_FILTER_NTP_ALL
};

std::map<std::string, std::vector<std::string>>
get_ptp_capabilities(const std::string &interface) {
  struct ethtool_command_context command_context {};

  // Check if interface has a valid length name.
  if (interface.length() >= IFNAMSIZ) {
    // TODO Handle this error with an exception throw.
  }
  command_context.devname = interface.c_str();
  memset(&command_context.ifr, 0, sizeof(command_context.ifr));
  strncpy(command_context.ifr.ifr_name, command_context.devname, interface.size());

  // Create a socket and retrieve its file descriptor.
  command_context.fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (command_context.fd < 0) {
    // TODO Handle this error with an exception throw.
  }

  // Create the command timestamp info request to kernel.
  struct ethtool_ts_info information_request {};
  information_request.cmd = ETHTOOL_GET_TS_INFO;

  // Send IOCTL request with the gathered information.
  command_context.ifr.ifr_data = &information_request;
  int error =
      ioctl(command_context.fd, IOCTL_ETHTOOL_REQ, &command_context.ifr);
  if (error < 0) {
    // TODO Handle this error with an exception throw.
  }

  // Parse the information left in the information request.
  std::map<std::string, std::vector<std::string>> information;

  std::vector<std::string> capabilities;
  for (int i = 0; i < timestamping_labels.size(); ++i) {
    if (information_request.so_timestamping & (1 << i)) {
      capabilities.push_back(timestamping_labels.at(i));
    }
  }
  information["capabilities"] = capabilities;

  std::vector<std::string> ptp_hardware_clock;
  if (information_request.phc_index > 0) {
    ptp_hardware_clock.push_back(std::to_string(information_request.phc_index));
  }
  information["ptp_hardware_clock"] = ptp_hardware_clock;

  std::vector<std::string> hardware_transmit_ts_modes;
  for (int i = 0; i < transmission_types.size(); ++i) {
    if (information_request.tx_types & (1 << i)) {
      hardware_transmit_ts_modes.push_back(transmission_types.at(i));
    }
  }
  information["hardware_transmit_ts_mode"] = hardware_transmit_ts_modes;

  std::vector<std::string> hardware_receive_filter_modes;
  for (int i = 0; i < reception_filters.size(); ++i) {
    if (information_request.rx_filters & (1 << i)) {
      hardware_receive_filter_modes.push_back(reception_filters.at(i));
    }
  }
  information["hardware_receive_filter_modes"] = hardware_receive_filter_modes;

  return information;
}