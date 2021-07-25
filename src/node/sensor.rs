use sysinfo::{ComponentExt, SystemExt};
use std::fmt::{Display, Formatter};
use serde::{Serialize, Deserialize};

#[derive(Debug, Serialize, Deserialize)]
pub struct Sensor {
    name: String,
    temperature: Option<f32>,
}

impl Sensor {
    pub fn update(sensor: &sysinfo::Component) -> Self {
        Sensor {
            name: String::from(sensor.label()),
            temperature: Some(sensor.temperature())
        }
    }

    pub fn update_all(system: &sysinfo::System) -> Option<Vec<Self>> {
        let mut sensors: Vec<Sensor> = Vec::new();

        if system.components().is_empty() {
            return None;
        }

        for sensor in system.components() {
            sensors.push(Sensor::update(sensor))
        }

        Some(sensors)
    }

    pub fn jsonify(&self) -> String {
        serde_json::to_string(self).unwrap()
    }
}

impl Display for Sensor {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "SEN -> [name: {}, temp: {}]", self.name, self.temperature.unwrap())
    }
}