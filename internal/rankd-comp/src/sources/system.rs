use uname::uname;
use cached::proc_macro::cached;

#[derive(Debug)]
pub struct SystemInformationSnapshot {
    /// Machine hostname value. The name of the host.
    hostname: String,
    /// Kernel release as an identifier version string.
    kernel_release: String,
    /// Kernel version as an identifier version number.
    kernel_text: String,
    /// Machine name from vendor.
    machine_name: String,
}

impl SystemInformationSnapshot {
    pub fn new() -> Self {
        SystemInformationSnapshot {
            hostname: hostname::get().unwrap().into_string().unwrap(),
            kernel_release: uname().unwrap().version,
            kernel_text: uname().unwrap().release,
            machine_name: uname().unwrap().machine
        }
    }

    pub fn hostname(&mut self) -> String {
        self.hostname = hostname::get().unwrap().into_string().unwrap();

        self.hostname.clone()
    }

    pub fn kernel_release(&mut self) -> String {
        self.kernel_release = uname().unwrap().version;

        self.kernel_release.clone()
    }

    pub fn kernel_text(&mut self) -> String {
        self.kernel_text = uname().unwrap().release;

        self.kernel_text.clone()
    }

    pub fn machine_name(&mut self) -> String {
        self.machine_name = uname().unwrap().machine;

        self.machine_name.clone()
    }
}

// Out-of-snapshot function definitions.

#[cached(time = 10)]
pub fn hostname() -> String {
    hostname::get().unwrap().into_string().unwrap()
}

#[cached(time = 10)]
pub fn kernel_release() -> String {
    uname().unwrap().version
}

#[cached(time = 10)]
pub fn kernel_text() -> String {
    uname().unwrap().release
}

#[cached(time = 10)]
pub fn machine_name() -> String {
    uname().unwrap().machine
}

#[cfg(test)]
mod tests {
    use crate::sources::system::*;

    #[test]
    fn get_system_information_snapshot() {
        let snapshot = SystemInformationSnapshot::new();

        println!("Testing a System Information snapshot with the following information:");
        println!("Hostname: {}", snapshot.hostname);
        println!("Kernel: {}, with text \n\t{}", snapshot.kernel_text, snapshot.kernel_release);
        println!("Machine: {}", snapshot.machine_name);
    }

    #[test]
    fn get_system_information() {
        println!("Testing a System Information snapshot with the following information:");
        println!("Hostname: {}", hostname());
        println!("Kernel: {}, with text \n\t{}", kernel_text(), kernel_release());
        println!("Machine: {}", machine_name());
    }
}
