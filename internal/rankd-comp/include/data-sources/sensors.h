#ifndef RANKD_SENSORS_H
#define RANKD_SENSORS_H

#define RAPIDJSON_HAS_STDSTRING 1

#include <map>
#include <string>
#include <utility>

#include "sensors-c++/sensors.h"

// RapidJSON inclusions.
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"

enum class BusType { ANY, I2C, ISA, PCI, SPI, VIRT, ACPI, HID, MDIO, SCSI };

BusType parse_bus_type(const sensors::bus_type &type);
std::string get_bus_name(const BusType& type);

enum class FeatureType {
  IN,
  FAN,
  TEMPERATURE,
  POWER,
  ENERGY,
  CURRENT,
  HUMIDITY,
  VID,
  INTRUSION,
  BEEP,
  UNKNOWN
};

FeatureType parse_feature_type(const sensors::feature_type &type);
std::string get_feature_name(const FeatureType& type);

enum class SubfeatureType {
  INPUT,
  INPUT_LOWEST,
  INPUT_HIGHEST,
  CAP,
  CAP_HYSTERESIS,
  CAP_ALARM,
  MIN,
  MIN_HYSTERESIS,
  MIN_ALARM,
  MAX,
  MAX_HYSTERESIS,
  MAX_ALARM,
  AVERAGE,
  LOWEST,
  HIGHEST,
  AVERAGE_LOWEST,
  AVERAGE_HIGHEST,
  AVERAGE_INTERVAL,
  CRITICAL,
  CRITICAL_HYSTERESIS,
  CRITICAL_ALARM,
  L_CRITICAL,
  L_CRITICAL_HYSTERESIS,
  L_CRITICAL_ALARM,
  ALARM,
  FAULT,
  EMERGENCY,
  EMERGENCY_HYSTERESIS,
  EMERGENCY_ALARM,
  TYPE,
  OFFSET,
  DIV,
  BEEP,
  PULSES,
  VID,
  ENABLE,
  UNKNOWN
};

SubfeatureType parse_subfeature_type(const sensors::subfeature_type &type);
std::string get_subfeature_name(const SubfeatureType& type);

class Subfeature {
public:
  Subfeature() = default;
  Subfeature(const std::string &name, int number,
             const sensors::subfeature_type &type, double value,
             bool is_compute_mapped, bool is_readable, bool is_writable) {
    _name = name;
    _number = number;
    _type = parse_subfeature_type(type);
    _value = value;
    _is_compute_mapped = is_compute_mapped;
    _is_readable = is_readable;
    _is_writable = is_writable;
  }
  [[nodiscard]] const std::string &name() const { return _name; }
  void name(const std::string &name) { _name = name; }
  [[nodiscard]] int number() const { return _number; }
  void number(int number) { _number = number; }
  [[nodiscard]] SubfeatureType type() const { return _type; }
  void type(SubfeatureType type) { _type = type; }
  [[nodiscard]] double value() const { return _value; }
  void value(double value) { _value = value; }
  [[nodiscard]] bool is_compute_mapped() const { return _is_compute_mapped; }
  void is_compute_mapped(bool is_compute_mapped) {
    _is_compute_mapped = is_compute_mapped;
  }
  [[nodiscard]] bool is_readable() const { return _is_readable; }
  void is_readable(bool is_readable) { _is_readable = is_readable; }
  [[nodiscard]] bool is_writable() const { return _is_writable; }
  void is_writable(bool is_writable) { _is_writable = is_writable; }

private:
  std::string _name;
  int _number{};
  SubfeatureType _type;
  double _value{};
  bool _is_compute_mapped{};
  bool _is_readable{};
  bool _is_writable{};
};

class Feature {
public:
  Feature() = default;
  Feature(const std::string &name, const sensors::feature_type &type) {
    _name = name;
    _type = parse_feature_type(type);
  }
  [[nodiscard]] const std::string &name() const { return _name; }
  void name(const std::string &name) { _name = name; }
  [[nodiscard]] FeatureType type() const { return _type; }
  void type(FeatureType type) { _type = type; }
  [[nodiscard]] const std::map<std::string, Subfeature> &subfeatures() const {
    return _subfeatures;
  }
  void subfeatures(const std::map<std::string, Subfeature> &subfeatures) {
    _subfeatures = subfeatures;
  }
  void update_subfeature(const std::string& key, Subfeature value) {
    _subfeatures[key] = std::move(value);
  }
private:
  std::string _name;
  FeatureType _type{};
  std::map<std::string, Subfeature> _subfeatures;
};

class Sensor {
public:
  Sensor() = default;
  Sensor(int address, const sensors::bus_type &type, int bus_number,
         std::string path, std::string prefix) {
    _address = address;
    _bus_type = parse_bus_type(type);
    _bus_number = bus_number;
    _path = std::move(path);
    _prefix = std::move(prefix);
  }
  [[nodiscard]] std::string name() {
    return get_bus_name(_bus_type) + "-" + _prefix + "-" + std::to_string(_address);
  }
  [[nodiscard]] int address() const { return _address; }
  void address(int address) { _address = address; }
  [[nodiscard]] BusType bus_type() const { return _bus_type; }
  void bus_type(BusType bus_type) { _bus_type = bus_type; }
  [[nodiscard]] int bus_number() const { return _bus_number; }
  void bus_number(int bus_number) { _bus_number = bus_number; }
  [[nodiscard]] const std::string &path() const { return _path; }
  void path(const std::string &path) { _path = path; }
  [[nodiscard]] const std::string &prefix() const { return _prefix; }
  void prefix(const std::string &prefix) { _prefix = prefix; }
  [[nodiscard]] const std::map<std::string, Feature> &features() const {
    return _features;
  }
  void features(const std::map<std::string, Feature> &features) {
    _features = features;
  }
  void update_features(const std::string& key, Feature value) {
    _features[key] = std::move(value);
  }
private:
  int _address{};
  BusType _bus_type;
  int _bus_number{};
  std::string _path;
  std::string _prefix;
  std::map<std::string, Feature> _features;
};

class Sensors {
public:
  Sensors();
  void snap();
  [[nodiscard]] const std::map<std::string, Sensor>& sensors() const { return _sensors; }
  void enable_json_output();
  void disable_json_output();
  [[nodiscard]] rapidjson::Document json() const;
  friend std::ostream& operator<<(std::ostream& os, const Sensors& sensors);
private:
  void update_sensors(const std::string& key, Sensor value) {
    _sensors[key] = std::move(value);
  }
  std::map<std::string, Sensor> _sensors;
  bool _json_formatted_output = false;
};

#endif // RANKD_SENSORS_H
