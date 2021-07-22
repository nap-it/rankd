use pnet::ipnetwork::IpNetwork;

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

#[derive(Debug)]
enum SDNRole {
    SWITCH,
    CONTROLLER,
}

#[derive(Debug)]
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

#[derive(Debug)]
struct Interface {
    name: String,
    ips: Vec<IpNetwork>,
}

#[derive(Debug)]
struct Network {
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