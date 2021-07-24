use pnet::ipnetwork::IpNetwork;
use validator::Contains;
use std::error::Error;

#[derive(Debug)]
struct Application {
    name: String,
    port: u16,
    version: String,
}

#[derive(Debug)]
struct Applications {
    network: Vec<Application>,
    av: Vec<Application>,
    utility: Vec<Application>,
}

impl Applications {
    pub fn new() -> Self {
        Self {
            network: Self::load_network_apps(),
            av: Self::load_av_apps(),
            utility: Self::load_utility_apps(),
        }
    }

    fn load_network_apps() -> Vec<Application> {
        Vec::new()
    }

    fn load_av_apps() -> Vec<Application> {
        Vec::new()
    }

    fn load_utility_apps() -> Vec<Application> {
        Vec::new()
    }
}

#[derive(Debug, Clone)]
enum SDNRole {
    SWITCH,
    CONTROLLER,
}

#[derive(Debug, Clone)]
struct SDN {
    role: SDNRole,
    framework: String,
    version: String,
}

impl SDN {
    pub fn new() -> Self {
        SDN {
            role: SDNRole::SWITCH,
            framework: String::from(""),
            version: String::from("")
        }
    }
}

#[derive(Debug, Clone)]
pub struct Interface {
    name: String,
    ips: Vec<IpNetwork>,
}

impl Contains for Interface {
    fn has_element(&self, needle: &str) -> bool {
        needle == self.name
    }
}

#[derive(Debug, Clone)]
pub struct Network {
    sdn: Option<SDN>,
    interfaces: Vec<Interface>,
}

impl Network {
    pub fn new() -> Self {
        Self {
            sdn: None,
            interfaces: Self::load_interfaces(),
        }
    }

    pub fn interfaces(&self) -> Vec<Interface> {
        self.interfaces.clone()
    }

    fn load_interfaces() -> Vec<Interface> {
        let mut interfaces: Vec<Interface> = Vec::new();
        for interface in pnet::datalink::interfaces() {
            interfaces.push(Interface {
                name: interface.name,
                ips: interface.ips,
            })
        }

        interfaces
    }
}

#[derive(Debug)]
pub enum Errors {
    NoSuchInterface,
}

#[derive(Debug)]
pub struct User {
    network: Network,
    applications: Applications,
}

impl User {
    pub fn new() -> Self {
        User {
            network: Network::new(),
            applications: Applications::new(),
        }
    }

    pub fn network(&self) -> Network {
        self.network.clone()
    }

    pub fn network_interface(&self, network_interface: String) -> Result<Interface, Errors> {
        let interfaces = crate::capabilities::Capabilities::user().network().interfaces();
        for interface in interfaces {
            if interface.has_element(network_interface.as_str()) {
                return Ok(interface);
            }
        }

        Err(Errors::NoSuchInterface)
    }
}

#[cfg(test)]
mod tests {
    use crate::capabilities::user::Network;

    #[test]
    fn create_network_object() {
        let network = Network::new();
        println!("{:#?}", network);
    }
}