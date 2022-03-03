use pnet::datalink::{interfaces, NetworkInterface};
use pnet::ipnetwork::IpNetwork;
use pnet::util::MacAddr;
use std::fmt::{Display, Formatter};
use sysinfo::{NetworkExt, SystemExt};

#[derive(Debug, Default)]
pub struct Network {
    /// The name of the interface.
    name: String,
    /// A description of the interface.
    description: String,
    /// The interface index (operating system specific).
    index: u32,
    /// A MAC address for the interface.
    mac_address: Option<MacAddr>,
    /// IP addresses and netmasks for the interface.
    ips: Vec<IpNetwork>,
    /// Flag telling if interface is up or not.
    is_up: bool,
    /// Flag telling if interface is loopback or not.
    is_loopback: bool,
    /// Flag telling if interface is P2P or not.
    is_point_to_point: bool,
    /// Flag telling if interface is multicast or not.
    is_multicast: bool,
    /// Total number of bytes received over interface.
    rx_bytes: u64,
    /// Total number of bytes transmitted over interface.
    tx_bytes: u64,
    /// Total number of packets received.
    rx_packets: u64,
    /// Total number of packets transmitted.
    tx_packets: u64,
}

impl Network {
    pub fn update(interface_name: &String, network: &sysinfo::NetworkData) -> Self {
        let interfaces = interfaces();
        let interface = interfaces
            .iter()
            .find(|int| int.name == *interface_name)
            .unwrap();

        Network {
            name: interface.name.to_owned(),
            description: interface.description.to_owned(),
            index: interface.index,
            mac_address: interface.mac,
            ips: interface.clone().ips,
            is_up: interface.is_up(),
            is_loopback: interface.is_loopback(),
            is_point_to_point: interface.is_point_to_point(),
            is_multicast: interface.is_multicast(),
            rx_bytes: network.received(),
            tx_bytes: network.transmitted(),
            rx_packets: network.packets_received(),
            tx_packets: network.packets_transmitted(),
        }
    }

    pub fn update_all(system: &sysinfo::System) -> Vec<Network> {
        let mut networks: Vec<Network> = Vec::new();

        for (interface, data) in system.networks() {
            networks.push(Network::update(interface, data))
        }

        networks
    }
}

impl Display for Network {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "IF  -> [name: {}, mac: {}, ips: ({})]",
            self.name,
            self.mac_address.unwrap(),
            {
                let mut ips = String::new();
                for ip in &self.ips {
                    ips += String::from(ip.ip().to_string() + "; ").as_str();
                }
                ips.pop();
                ips.pop();
                ips
            }
        )
    }
}

#[cfg(test)]
mod tests {
    use sysinfo::SystemExt;
    use crate::node::network::Network;

    #[test]
    fn create_network_object() {
        let system = sysinfo::System::new_all();
        let networks = Network::update_all(&system);

        println!("{:?}", networks);
        for network in networks {
            println!("{}", network);
        }
    }
}