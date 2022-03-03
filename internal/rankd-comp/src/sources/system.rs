#[derive(Debug)]
pub struct SystemInformationSnapshot {
    /// Machine hostname value. The name of the machine.
    hostname: String,
    kernel_release: String,
    kernel_version: String,
    machine_name: String,
    system_version: String,

}

impl SystemInformationSnapshot {
    #[cached(time = 10)]
    pub fn hostname(&mut self) -> String {
        self.hostname = hostname::get().unwrap().into_string().unwrap();

        self.hostname.clone()
    }
}

// Out-of-snapshot function definitions.

#[cached(time = 10)]
pub fn hostname() -> String {
    hostname::get().unwrap().into_string().unwrap()
}