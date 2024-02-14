#include "data-sources/sensors.h"

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

void Sensors::enable_json_output() {
    _json_formatted_output = true;
}

rapidjson::Document Sensors::json() const {
  // Create a JSON document.
  rapidjson::Document json_document;
  json_document.SetObject();
  auto& allocator = json_document.GetAllocator();

  // Create temporary value.
  rapidjson::Value value;

  for (const auto& [name, sensor] : _sensors) {
    value.SetObject();
    json_document.AddMember(rapidjson::GenericStringRef(name.c_str()), value, allocator);
    value.SetInt(sensor.address());
    json_document[name.c_str()].AddMember("address", value, allocator);

    switch (sensor.bus_type()) {
      case BusType::ANY:
        value.SetString("any"); break;
      case BusType::I2C:
        value.SetString("i2c"); break;
      case BusType::ISA:
        value.SetString("isa"); break;
      case BusType::PCI:
        value.SetString("pci"); break;
      case BusType::SPI:
        value.SetString("spi"); break;
      case BusType::VIRT:
        value.SetString("virt"); break;
      case BusType::ACPI:
        value.SetString("acpi"); break;
      case BusType::HID:
        value.SetString("hid"); break;
      case BusType::MDIO:
        value.SetString("mdio"); break;
      case BusType::SCSI:
        value.SetString("scsi"); break;
    }
    json_document[name.c_str()].AddMember("bus-type", value, allocator);

    value.SetString(rapidjson::GenericStringRef(sensor.path().c_str()));
    json_document[name.c_str()].AddMember("path", value, allocator);
    value.SetString(rapidjson::GenericStringRef(sensor.prefix().c_str()));
    json_document[name.c_str()].AddMember("prefix", value, allocator);

    value.SetObject();
    json_document[name.c_str()].AddMember("features", value, allocator);
    for (const auto& [topic, topic_value] : sensor.features()) {
      value.SetObject();
      json_document[name.c_str()]["features"].AddMember(rapidjson::GenericStringRef(topic.c_str()), value, allocator);

      switch (topic_value.type()) {
        case FeatureType::IN:
          value.SetString("in"); break;
        case FeatureType::FAN:
          value.SetString("fan"); break;
        case FeatureType::TEMPERATURE:
          value.SetString("temperature"); break;
        case FeatureType::POWER:
          value.SetString("power"); break;
        case FeatureType::ENERGY:
          value.SetString("energy"); break;
        case FeatureType::CURRENT:
          value.SetString("current"); break;
        case FeatureType::HUMIDITY:
          value.SetString("humidity"); break;
        case FeatureType::VID:
          value.SetString("vid"); break;
        case FeatureType::INTRUSION:
          value.SetString("intrusion"); break;
        case FeatureType::BEEP:
          value.SetString("beep"); break;
        case FeatureType::UNKNOWN:
          value.SetString("unknown"); break;
      }
      json_document[name.c_str()]["features"][topic.c_str()].AddMember("type", value, allocator);

      value.SetObject();
    json_document[name.c_str()]["features"][topic.c_str()].AddMember("subfeatures", value, allocator);
      for (const auto& [subtopic, subvalue] : topic_value.subfeatures()) {
        value.SetObject();
        json_document[name.c_str()]["features"][topic.c_str()]["subfeatures"].AddMember(rapidjson::GenericStringRef(subtopic.c_str()), value, allocator);
        value.SetInt(subvalue.number());
        json_document[name.c_str()]["features"][topic.c_str()]["subfeatures"][subtopic.c_str()].AddMember("number", value, allocator);

        switch (subvalue.type()) {
          case SubfeatureType::INPUT:
              value.SetString("input"); break;
          case SubfeatureType::INPUT_LOWEST:
              value.SetString("input lowest"); break;
          case SubfeatureType::INPUT_HIGHEST:
              value.SetString("input highest"); break;
          case SubfeatureType::CAP:
              value.SetString("cap"); break;
          case SubfeatureType::CAP_HYSTERESIS:
              value.SetString("cap hysteresis"); break;
          case SubfeatureType::CAP_ALARM:
              value.SetString("cap alarm"); break;
          case SubfeatureType::MIN:
              value.SetString("min"); break;
          case SubfeatureType::MIN_HYSTERESIS:
              value.SetString("min hysteresis"); break;
          case SubfeatureType::MIN_ALARM:
              value.SetString("min alarm"); break;
          case SubfeatureType::MAX:
              value.SetString("max"); break;
          case SubfeatureType::MAX_HYSTERESIS:
              value.SetString("max hysteresis"); break;
          case SubfeatureType::MAX_ALARM:
              value.SetString("max alarm"); break;
          case SubfeatureType::AVERAGE:
              value.SetString("average"); break;
          case SubfeatureType::LOWEST:
              value.SetString("lowest"); break;
          case SubfeatureType::HIGHEST:
              value.SetString("highest"); break;
          case SubfeatureType::AVERAGE_LOWEST:
              value.SetString("average lowest"); break;
          case SubfeatureType::AVERAGE_HIGHEST:
              value.SetString("average highest"); break;
          case SubfeatureType::AVERAGE_INTERVAL:
              value.SetString("average interval"); break;
          case SubfeatureType::CRITICAL:
              value.SetString("critical"); break;
          case SubfeatureType::CRITICAL_HYSTERESIS:
              value.SetString("critical hysteresis"); break;
          case SubfeatureType::CRITICAL_ALARM:
              value.SetString("critical alarm"); break;
          case SubfeatureType::L_CRITICAL:
              value.SetString("l critical"); break;
          case SubfeatureType::L_CRITICAL_HYSTERESIS:
              value.SetString("l critical hysteresis"); break;
          case SubfeatureType::L_CRITICAL_ALARM:
              value.SetString("l critical alarm"); break;
          case SubfeatureType::ALARM:
              value.SetString("alarm"); break;
          case SubfeatureType::FAULT:
              value.SetString("fault"); break;
          case SubfeatureType::EMERGENCY:
              value.SetString("emergency"); break;
          case SubfeatureType::EMERGENCY_HYSTERESIS:
              value.SetString("emergency hysteresis"); break;
          case SubfeatureType::EMERGENCY_ALARM:
              value.SetString("emergency alarm"); break;
          case SubfeatureType::TYPE:
              value.SetString("type"); break;
          case SubfeatureType::OFFSET:
              value.SetString("offset"); break;
          case SubfeatureType::DIV:
              value.SetString("div"); break;
          case SubfeatureType::BEEP:
              value.SetString("beep"); break;
          case SubfeatureType::PULSES:
              value.SetString("pulses"); break;
          case SubfeatureType::VID:
              value.SetString("vid"); break;
          case SubfeatureType::ENABLE:
              value.SetString("enable"); break;
          case SubfeatureType::UNKNOWN:
              value.SetString("unknown"); break;
        }
        json_document[name.c_str()]["features"][topic.c_str()]["subfeatures"][subtopic.c_str()].AddMember("type", value, allocator);

        value.SetDouble(subvalue.value());
        json_document[name.c_str()]["features"][topic.c_str()]["subfeatures"][subtopic.c_str()].AddMember("value", value, allocator);
        value.SetBool(subvalue.is_compute_mapped());
        json_document[name.c_str()]["features"][topic.c_str()]["subfeatures"][subtopic.c_str()].AddMember("is-compute-mapped", value, allocator);
        value.SetBool(subvalue.is_readable());
        json_document[name.c_str()]["features"][topic.c_str()]["subfeatures"][subtopic.c_str()].AddMember("is-readable", value, allocator);
        value.SetBool(subvalue.is_writable());
        json_document[name.c_str()]["features"][topic.c_str()]["subfeatures"][subtopic.c_str()].AddMember("is-writable", value, allocator);
      }
    }
  }

  return json_document;
}

std::ostream& operator<<(std::ostream& os, const Sensors& sensors) {
    if (sensors._json_formatted_output) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        auto json_document = sensors.json();
        json_document.Accept(writer);

        os << buffer.GetString();
    } else {
        for (const auto& [name, sensor]: sensors._sensors) {
            os << name << " sensor: " << "\n";
            os << "\t" << "Bus Type: " << get_bus_name(sensor.bus_type()) << "\n";
            os << "\t" << "Features: " << "\n";
            for (const auto& [feature_name, feature]: sensor.features()) {
                os << "\t -" << "Name: " << feature_name << "\n";
                os << "\t  " << "Type: " << get_feature_name(feature.type()) << "\n";
                os << "\t  " << "Value: ";
                for (const auto& [_, subfeature] : feature.subfeatures()) {
                    if (subfeature.type() == SubfeatureType::INPUT) {
                        if (feature.type() == FeatureType::TEMPERATURE) { os << subfeature.value() << "ºC "; }
                        else if (feature.type() == FeatureType::FAN) { os << subfeature.value() << " rpm "; }
                        else if (feature.type() == FeatureType::IN) { os << subfeature.value() << " V "; }
                        break;
                    }
                }
                for (const auto& [_, subfeature] : feature.subfeatures()) {
                    if (subfeature.type() == SubfeatureType::CRITICAL) {
                        if (feature.type() == FeatureType::TEMPERATURE) { os << "out of " << subfeature.value() << "ºC"; }
                        else if (feature.type() == FeatureType::FAN) { os << "out of " << subfeature.value() << " rpm"; }
                        break;
                    }
                }
                os << "\n";
            }
        }
        os << std::endl;
    }

  return os;
}
