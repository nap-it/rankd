use os_info::Version;
use std::io::Error;
use systemstat::Platform;
use std::time::Duration;

#[derive(Debug)]
pub(crate) struct Kernel {
    name: String,
    nodename: String,
    release: String,
    version: String,
    machine: String,
    os: String,
    os_version: String,
    precision: u8,
    uptime: Option<Duration>,
}

impl Kernel {
    pub fn new() -> Self {
        let uname_structure = uname::uname().unwrap();
        println!("{:?}", uname_structure);
        Kernel {
            name: uname_structure.sysname,
            nodename: uname_structure.nodename,
            release: uname_structure.release,
            version: uname_structure.version,
            machine: uname_structure.machine,
            os: {
                let codename = match os_info::get().codename() {
                    None => String::from(""),
                    Some(name) => String::from(format!(" {}", name)),
                };
                String::from(format!("{}{}", os_info::get().os_type().to_string(), codename))
            },
            os_version: {
                match os_info::get().version() {
                    Version::Unknown => String::from("unknown"),
                    Version::Semantic(major, minor, patch) => String::from(format!("{}.{}.{}", major, minor, patch)),
                    Version::Rolling(version) => version.as_ref().unwrap().to_string(),
                    Version::Custom(version) => version.to_string(),
                }
            },
            precision: std::mem::size_of::<&char>() as u8,
            uptime: {
                match systemstat::System::new().uptime() {
                    Ok(duration) => Some(duration), 
                    Err(_) => None,
                }
            },
        }
    }
}

#[cfg(test)]
mod tests {
    use crate::capabilities::kernel::Kernel;

    #[test]
    fn create_kernel_object() {
        let kernel = Kernel::new();
        println!("{:#?}", kernel);
        assert_eq!(kernel.precision, 8);
    }
}