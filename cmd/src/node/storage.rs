use std::fmt::{Display, Formatter};
use sysinfo::{DiskExt, DiskType, SystemExt};
use serde::{Serialize, Deserialize};

#[derive(Debug, Serialize, Deserialize)]
pub enum StorageType {
    /// Hard disk drive.
    HDD,
    /// Solid-state drive.
    SSD,
    /// Other unknown type.
    Unknown(isize),
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Storage {
    type_: StorageType,
    name: String,
    file_system: String,
    mount_point: String,
    total: u64,
    available: u64,
    is_removable: bool,
}

impl Storage {
    pub fn update(disk: &sysinfo::Disk) -> Self {
        Storage {
            type_: match disk.type_() {
                DiskType::HDD => StorageType::HDD,
                DiskType::SSD => StorageType::SSD,
                DiskType::Unknown(value) => StorageType::Unknown(value),
            },
            name: String::from(disk.name().to_str().unwrap()),
            file_system: String::from_utf8(disk.file_system().to_vec()).unwrap(),
            mount_point: disk.mount_point().to_str().unwrap().to_string(),
            total: disk.total_space(),
            available: disk.available_space(),
            is_removable: disk.is_removable(),
        }
    }

    pub fn update_all(system: &sysinfo::System) -> Vec<Storage> {
        let mut storages: Vec<Storage> = Vec::new();

        for storage in system.disks() {
            storages.push(Storage::update(storage));
        }

        storages
    }

    pub fn jsonify(&self) -> String {
        serde_json::to_string(self).unwrap()
    }
}

impl Display for Storage {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "DSK -> [name: {}, type: {}, fs: {}, at: {}, rem: {}, free: {}, total: {}]",
            self.name,
            match self.type_ {
                StorageType::HDD => "HDD",
                StorageType::SSD => "SSD",
                StorageType::Unknown(_) => "Unknown"
            },
            self.file_system,
            self.mount_point,
            self.is_removable,
            self.available,
            self.total
        )
    }
}

#[cfg(test)]
mod tests {
    use sysinfo::SystemExt;
    use crate::node::storage::Storage;

    #[test]
    fn create_storage_object() {
        let system = sysinfo::System::new_all();
        let storages = Storage::update_all(&system);

        println!("{:?}", storages);

        for storage in storages {
            println!("{}", storage);
        }
    }
}
