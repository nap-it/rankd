use std::fmt::{Display, Formatter};
use sysinfo::SystemExt;

#[derive(Debug, Serialize, Deserialize)]
pub struct Memory {
    /// Total RAM in KB.
    total: u64,
    /// Free RAM in KB.
    free: u64,
    /// Available RAM in KB.
    ///
    /// Generally, “free” memory refers to unallocated memory whereas “available” memory refers to memory that is available for (re)use.
    available: u64,
    /// Used RAM in KB.
    used: u64,
    /// Total size of swap memory, in KB.
    swap_total: u64,
    /// Free size of swap memory, in KB.
    swap_free: u64,
    /// Used size of swap memory, in KB.
    swap_used: u64,
}

impl Memory {
    pub fn update(system: &sysinfo::System) -> Self {
        Memory {
            total: system.total_memory(),
            free: system.free_memory(),
            available: system.available_memory(),
            used: system.used_memory(),
            swap_total: system.total_swap(),
            swap_free: system.free_swap(),
            swap_used: system.used_swap(),
        }
    }

    pub fn jsonify(&self) -> String {
        serde_json::to_string(self).unwrap()
    }
}

impl Display for Memory {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "MEM -> [free: {}, used: {}, total: {}, swap( free: {}, used: {}, total: {} )]",
            self.free, self.used, self.total, self.swap_free, self.swap_used, self.swap_total
        )
    }
}
