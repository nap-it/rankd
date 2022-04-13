#include <iostream>
#include "structs/network.h"

Network::Network() {
  std::ifstream net_file("/proc/net/dev");

  std::string line;
  getline(net_file, line);
  getline(net_file, line);

  while (!net_file.eof()) {
    getline(net_file, line, ':');
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

    InterfaceStats stats;
    net_file >> stats.rx_bytes;
    net_file >> stats.rx_packets;

    int ignore_values;
    net_file >> ignore_values >> ignore_values >> ignore_values >> ignore_values >> ignore_values >> ignore_values;

    net_file >> stats.tx_bytes;
    net_file >> stats.tx_packets;

    net_file >> ignore_values >> ignore_values >> ignore_values >> ignore_values >> ignore_values >> ignore_values;

    _interface_stats[line] = stats;

    getline(net_file, line);
  }

  net_file.close();
}

void Network::snap() {
  auto network = pfs::procfs().get_net();
}