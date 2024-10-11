#include "data-sources/sensors.h"

std::string get_bus_name(const BusType& type) {
  switch (type) {
  case BusType::ANY:
    return "Unspecified interface. Considered as Any.";
  case BusType::I2C:
    return "I2C";
  case BusType::ISA:
    return "ISA";
  case BusType::PCI:
    return "PCI";
  case BusType::SPI:
    return "SPI";
  case BusType::VIRT:
    return "VIRT";
  case BusType::ACPI:
    return "ACPI";
  case BusType::HID:
    return "HID";
  case BusType::MDIO:
    return "MDIO";
  case BusType::SCSI:
    return "SCSI";
  }
}

std::string get_feature_name(const FeatureType& type) {
  switch (type) {
  case FeatureType::IN:
    return "In";
  case FeatureType::FAN:
    return "Fan";
  case FeatureType::TEMPERATURE:
    return "Temperature";
  case FeatureType::POWER:
    return "Power";
  case FeatureType::ENERGY:
    return "Energy";
  case FeatureType::CURRENT:
    return "Current";
  case FeatureType::HUMIDITY:
    return "Humidity";
  case FeatureType::VID:
    return "Vid";
  case FeatureType::INTRUSION:
    return "Intrusion";
  case FeatureType::BEEP:
    return "Beep";
  case FeatureType::UNKNOWN:
    return "Unknown";
  }
}

std::string get_subfeature_name(const SubfeatureType& type) {
  switch (type) {
  case SubfeatureType::INPUT:
    return "Input";
  case SubfeatureType::INPUT_LOWEST:
    return "Input (lowest value)";
  case SubfeatureType::INPUT_HIGHEST:
    return "Input (highest value)";
  case SubfeatureType::CAP:
    return "Cap";
  case SubfeatureType::CAP_HYSTERESIS:
    return "Cap (hysteresis value)";
  case SubfeatureType::CAP_ALARM:
    return "Cap (alarm value)";
  case SubfeatureType::MIN:
    return "Minimum";
  case SubfeatureType::MIN_HYSTERESIS:
    return "Minimum (hysteresis value)";
  case SubfeatureType::MIN_ALARM:
    return "Minimum (alarm value)";
  case SubfeatureType::MAX:
    return "Maximum";
  case SubfeatureType::MAX_HYSTERESIS:
    return "Maximum (hysteresis value)";
  case SubfeatureType::MAX_ALARM:
    return "Maximum (alarm value)";
  case SubfeatureType::AVERAGE:
    return "Average";
  case SubfeatureType::LOWEST:
    return "Lowest";
  case SubfeatureType::HIGHEST:
    return "Highest";
  case SubfeatureType::AVERAGE_LOWEST:
    return "Average (lowest value)";
  case SubfeatureType::AVERAGE_HIGHEST:
    return "Average (highest value)";
  case SubfeatureType::AVERAGE_INTERVAL:
    return "Average (interval)";
  case SubfeatureType::CRITICAL:
    return "Critical";
  case SubfeatureType::CRITICAL_HYSTERESIS:
    return "Critical (hysteresis value)";
  case SubfeatureType::CRITICAL_ALARM:
    return "Critical (alarm value)";
  case SubfeatureType::L_CRITICAL:
    return "Low Critical";
  case SubfeatureType::L_CRITICAL_HYSTERESIS:
    return "Low Critical (hysteresis value)";
  case SubfeatureType::L_CRITICAL_ALARM:
    return "Low Critical (alarm value)";
  case SubfeatureType::ALARM:
    return "Alarm";
  case SubfeatureType::FAULT:
    return "Fault";
  case SubfeatureType::EMERGENCY:
    return "Emergency";
  case SubfeatureType::EMERGENCY_HYSTERESIS:
    return "Emergency (hysteresis value)";
  case SubfeatureType::EMERGENCY_ALARM:
    return "Emergency (alarm value)";
  case SubfeatureType::TYPE:
    return "Type";
  case SubfeatureType::OFFSET:
    return "Offset";
  case SubfeatureType::DIV:
    return "Div";
  case SubfeatureType::BEEP:
    return "Beep";
  case SubfeatureType::PULSES:
    return "Pulses";
  case SubfeatureType::VID:
    return "Vid";
  case SubfeatureType::ENABLE:
    return "Enable";
  case SubfeatureType::UNKNOWN:
    return "Unknown";
  }
}

Sensors::Sensors() {
    // Initialize all the variables for future snapping.
    _sensors = {};

    // Take a snapshot of the system, CPU-wise.
    snap();
}

void Sensors::snap() {
    // TODO
}
