#ifndef RANKD_NETWORK_H
#define RANKD_NETWORK_H

#include <algorithm>
#include <fstream>
#include <map>

#include "pfs/procfs.hpp"

struct InterfaceStats {
  unsigned long long int tx_packets;
  unsigned long long int rx_packets;
  unsigned long long int tx_bytes;
  unsigned long long int rx_bytes;
};

class Network {
public:
  Network();
  void snap();
private:
  std::map<std::string, InterfaceStats> _interface_stats;
  std::map<std::string, std::pair<std::string, std::string>> _arp_table;
  int _tcp_allocated_sockets;
  int _tcp_inuse_sockets;
  int _tcp_orphan_sockets;
  int _tcp_tw_sockets;
  int _tcp_in_data;
  int _tcp_out_data;
  int _udp_inuse_sockets;
  int _udp_udplite_inuse_sockets;
  int _udp_mem_sockets;
  int _udp_in_data;
  int _udp_out_data;
  int _sockets_used;
  bool _ip_forwarding;
  int _icmp_out;
  int _icmp_in;
  int _icmp_errors;
};

#endif // RANKD_NETWORK_H
