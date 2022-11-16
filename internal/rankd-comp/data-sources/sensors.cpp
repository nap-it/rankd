#include "sensors.h"

BusType parse_bus_type(const sensors::bus_type &type) {
  switch (type) {
  case sensors::bus_type::any:
    return BusType::ANY;
  case sensors::bus_type::i2c:
    return BusType::I2C;
  case sensors::bus_type::isa:
    return BusType::ISA;
  case sensors::bus_type::pci:
    return BusType::PCI;
  case sensors::bus_type::spi:
    return BusType::SPI;
  case sensors::bus_type::virt:
    return BusType::VIRT;
  case sensors::bus_type::acpi:
    return BusType::ACPI;
  case sensors::bus_type::hid:
    return BusType::HID;
  case sensors::bus_type::mdio:
    return BusType::MDIO;
  case sensors::bus_type::scsi:
    return BusType::SCSI;
  }
}

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

FeatureType parse_feature_type(const sensors::feature_type &type) {
  switch (type) {
  case sensors::feature_type::in:
    return FeatureType::IN;
  case sensors::feature_type::fan:
    return FeatureType::FAN;
  case sensors::feature_type::temp:
    return FeatureType::TEMPERATURE;
  case sensors::feature_type::power:
    return FeatureType::POWER;
  case sensors::feature_type::energy:
    return FeatureType::ENERGY;
  case sensors::feature_type::current:
    return FeatureType::CURRENT;
  case sensors::feature_type::humidity:
    return FeatureType::HUMIDITY;
  case sensors::feature_type::vid:
    return FeatureType::VID;
  case sensors::feature_type::intrusion:
    return FeatureType::INTRUSION;
  case sensors::feature_type::beep:
    return FeatureType::BEEP;
  case sensors::feature_type::unknown:
    return FeatureType::UNKNOWN;
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

SubfeatureType parse_subfeature_type(const sensors::subfeature_type &type) {
  switch (type) {
  case sensors::subfeature_type::input:
    return SubfeatureType::INPUT;
  case sensors::subfeature_type::input_lowest:
    return SubfeatureType::INPUT_LOWEST;
  case sensors::subfeature_type::input_highest:
    return SubfeatureType::INPUT_HIGHEST;
  case sensors::subfeature_type::cap:
    return SubfeatureType::CAP;
  case sensors::subfeature_type::cap_hyst:
    return SubfeatureType::CAP_HYSTERESIS;
  case sensors::subfeature_type::cap_alarm:
    return SubfeatureType::CAP_ALARM;
  case sensors::subfeature_type::min:
    return SubfeatureType::MIN;
  case sensors::subfeature_type::min_hyst:
    return SubfeatureType::MIN_HYSTERESIS;
  case sensors::subfeature_type::min_alarm:
    return SubfeatureType::MIN_ALARM;
  case sensors::subfeature_type::max:
    return SubfeatureType::MAX;
  case sensors::subfeature_type::max_hyst:
    return SubfeatureType::MAX_HYSTERESIS;
  case sensors::subfeature_type::max_alarm:
    return SubfeatureType::MAX_ALARM;
  case sensors::subfeature_type::average:
    return SubfeatureType::AVERAGE;
  case sensors::subfeature_type::lowest:
    return SubfeatureType::LOWEST;
  case sensors::subfeature_type::highest:
    return SubfeatureType::HIGHEST;
  case sensors::subfeature_type::average_lowest:
    return SubfeatureType::AVERAGE_LOWEST;
  case sensors::subfeature_type::average_highest:
    return SubfeatureType::AVERAGE_HIGHEST;
  case sensors::subfeature_type::average_interval:
    return SubfeatureType::AVERAGE_INTERVAL;
  case sensors::subfeature_type::crit:
    return SubfeatureType::CRITICAL;
  case sensors::subfeature_type::crit_hyst:
    return SubfeatureType::CRITICAL_HYSTERESIS;
  case sensors::subfeature_type::crit_alarm:
    return SubfeatureType::CRITICAL_ALARM;
  case sensors::subfeature_type::l_crit:
    return SubfeatureType::L_CRITICAL;
  case sensors::subfeature_type::l_crit_hyst:
    return SubfeatureType::L_CRITICAL_HYSTERESIS;
  case sensors::subfeature_type::l_crit_alarm:
    return SubfeatureType::L_CRITICAL_ALARM;
  case sensors::subfeature_type::alarm:
    return SubfeatureType::ALARM;
  case sensors::subfeature_type::fault:
    return SubfeatureType::FAULT;
  case sensors::subfeature_type::emergency:
    return SubfeatureType::EMERGENCY;
  case sensors::subfeature_type::emergency_hyst:
    return SubfeatureType::EMERGENCY_HYSTERESIS;
  case sensors::subfeature_type::emergency_alarm:
    return SubfeatureType::EMERGENCY_ALARM;
  case sensors::subfeature_type::type:
    return SubfeatureType::TYPE;
  case sensors::subfeature_type::offset:
    return SubfeatureType::OFFSET;
  case sensors::subfeature_type::div:
    return SubfeatureType::DIV;
  case sensors::subfeature_type::beep:
    return SubfeatureType::BEEP;
  case sensors::subfeature_type::pulses:
    return SubfeatureType::PULSES;
  case sensors::subfeature_type::vid:
    return SubfeatureType::VID;
  case sensors::subfeature_type::enable:
    return SubfeatureType::ENABLE;
  case sensors::subfeature_type::unknown:
    return SubfeatureType::UNKNOWN;
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
  snap();
}

void Sensors::snap() {
  // Get all the chips information on the system, by using libsensors++.
  auto chips = sensors::get_detected_chips();

  // If there is no information retrived from the library, manually attempt it.
  if (chips.empty()) {

  } else {
    for (auto& chip : chips) {
      // Create sensor information.
      Sensor sensor = Sensor(
          chip.address(),
          chip.bus().type(),
          chip.bus().nr(),
          std::string(chip.path()),
          std::string(chip.prefix()));

      for (auto& feature : chip.features()) {
        // Create feature information.
        Feature temporary_feature = Feature(std::string(feature.name()), feature.type());

        for (auto& subfeature : feature.subfeatures()) {
          // Create subfeature information.
          Subfeature temporary_subfeature = Subfeature(
              std::string(subfeature.name()),
              subfeature.number(),
              subfeature.type(),
              subfeature.read(),
              subfeature.compute_mapping(),
              subfeature.readable(),
              subfeature.writable()
              );

          // Add subfeature to feature.
          temporary_feature.update_subfeature(temporary_subfeature.name(), temporary_subfeature);
        }

        // Add feature to sensor.
        sensor.update_features(temporary_feature.name(), temporary_feature);
      }

      // Add sensor to sensors.
      update_sensors(sensor.path(), sensor);
    }
  }
}