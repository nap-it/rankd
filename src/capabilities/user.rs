use pnet::ipnetwork::IpNetwork;

struct Application {
    name: String,
    port: u16,
    version: String,
}

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
        unimplemented!()
    }

    fn load_av_apps() -> Vec<Application> {
        unimplemented!()
    }

    fn load_utility_apps() -> Vec<Application> {
        unimplemented!()
    }
}

enum SDNRole {
    SWITCH,
    CONTROLLER,
}

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

struct Interface {
    name: String,
    ips: Vec<IpNetwork>,
}

struct Network {
    sdn: SDN,
    interfaces: Vec<Interface>,
}

impl Network {
    pub fn new() -> Self {
        Self {
            sdn: SDN::new(),
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

pub(crate) struct User {
    network: Network,
    applications: Applications,
}

impl User {
    pub fn new(&self) -> Self {
        User {
            network: Network::new(),
            applications: Applications::new(),
        }
    }
}

