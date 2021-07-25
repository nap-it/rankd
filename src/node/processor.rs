use std::fmt::{Display, Formatter};
use sysinfo::{ProcessorExt, SystemExt};
use serde::{Serialize, Deserialize};

#[derive(Debug, Serialize, Deserialize)]
pub struct Processor {
    frequency: u64,
    cpu_usage: f32,
    name: String,
    vendor_id: String,
    brand: String,
}

impl Processor {
    pub fn update(processor: &sysinfo::Processor) -> Self {
        Processor {
            frequency: processor.frequency(),
            cpu_usage: processor.cpu_usage(),
            name: processor.name().to_owned(),
            vendor_id: processor.vendor_id().to_owned(),
            brand: processor.brand().to_owned(),
        }
    }

    pub fn update_all(system: &sysinfo::System) -> Vec<Processor> {
        let mut processors: Vec<Processor> = Vec::new();

        for processor in system.processors() {
            processors.push(Self::update(processor));
        }

        processors
    }

    pub fn jsonify(&self) -> String {
        serde_json::to_string(self).unwrap()
    }
}

impl Display for Processor {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "CPU -> [name: {}, vendor_id: {}, brand: {}, frequency: {}, cpu_usage: {}]",
            self.name, self.vendor_id, self.brand, self.frequency, self.cpu_usage
        )
    }
}
