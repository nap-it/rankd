use crate::node::graphics::Graphics;
use crate::node::sensor::Sensor;
use serde::{Deserialize, Serialize};
use std::fmt::{Display, Formatter};
use sysinfo::SystemExt;

mod graphics;
mod memory;
mod network;
mod os;
mod processor;
mod sensor;
mod storage;

#[derive(Debug, Serialize, Deserialize)]
pub struct SystemInformation {
    /// Host name as revealed in the current network.
    host_name: Option<String>,
    /// Kernel version.
    kernel_version: Option<String>,
    /// Operative system version (not kernel's).
    os_version: Option<String>,
    /// Operative system long version name (e.g “MacOS 11.2 BigSur”).
    long_os_version: Option<String>,
    /// System name.
    name: Option<String>,
    /// System uptime (in seconds).
    uptime: u64,
    /// Time (in seconds) when the system booted, since UNIX epoch.
    boot_time: u64,
    /// Number of physical cores on the system.
    cores: Option<usize>,
}

impl SystemInformation {
    /// Update the information on the system information of a node, with the help of a `sysinfo::System` object.
    ///
    /// # Arguments
    ///
    /// * `system` - A `sysinfo::System` object which grants an easy way to access data on the system.
    ///
    /// # Examples
    ///
    /// ```
    /// // You can update the information given the `sysinfo::System` object that already
    /// // resides in the `System` structure, within its implementation.
    /// let information = SystemInformation::update(self._system);
    /// ```
    ///
    /// Instead, you can also use this structure in the outside environments, just invoking this method with a proper object.
    ///
    /// ```
    /// let s = sysinfo::System::new();
    /// let information = SystemInformation::update(s);
    /// ```
    pub fn update(system: &sysinfo::System) -> Self {
        SystemInformation {
            host_name: system.host_name(),
            kernel_version: system.kernel_version(),
            os_version: system.os_version(),
            long_os_version: system.long_os_version(),
            name: system.name(),
            uptime: system.uptime(),
            boot_time: system.boot_time(),
            cores: system.physical_core_count(),
        }
    }

    /// Gives a JSON object written as String from the internal structure.
    pub fn jsonify(&self) -> String {
        serde_json::to_string(self).unwrap()
    }
}

impl Display for SystemInformation {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "[host_name: {}, os: {}, kernel: {}, uptime: {}]",
            self.host_name.as_ref().unwrap(),
            self.long_os_version.as_ref().unwrap(),
            self.kernel_version.as_ref().unwrap(),
            self.uptime
        )
    }
}

#[derive(Debug)]
pub struct System {
    /// Global and summary information on the system.
    pub global_information: SystemInformation,
    pub processor: Vec<processor::Processor>,
    pub memory: memory::Memory,
    pub storage: Vec<storage::Storage>,
    pub network: Vec<network::Network>,
    pub os: os::OperativeSystem,
    pub graphics: Option<graphics::Graphics>,
    pub sensors: Option<Vec<Sensor>>,
}

impl System {
    pub fn new() -> Self {
        let system = sysinfo::System::new_all();

        System {
            global_information: SystemInformation::update(&system),
            processor: processor::Processor::update_all(&system),
            memory: memory::Memory::update(&system),
            storage: storage::Storage::update_all(&system),
            network: network::Network::update_all(&system),
            os: os::OperativeSystem::update(&system),
            graphics: Graphics::load(),
            sensors: Sensor::update_all(&system),
        }
    }
}
