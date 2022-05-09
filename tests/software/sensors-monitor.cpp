#include <iostream>
#include <string>

#include "comp-lib.h"
#include "sensors-c++/sensors.h"

using namespace std;

int main() {
  Sensors sensors = Sensors();

  cout << "A total of " << sensors.sensors().size() << " sensors were found." << endl;
  cout << endl;

  for (auto [sensor_name, sensor] : sensors.sensors()) {
    cout << "A sensor called \"" << sensor_name << "\" was found with the following info:" << endl;
    cout << "Name: " << sensor.name() << endl;
    cout << "Address: " << sensor.address() << endl;
    cout << "Bus: " << get_bus_name(sensor.bus_type()) << endl;
    cout << "Path: " << sensor.path() << endl;
    cout << "Prefix: " << sensor.prefix() << endl;

    cout << " The following features are presented:" << endl;
    for (auto [feature_name, feature] : sensor.features()) {
      cout << "\t" << "Feature identifier: " << feature_name << endl;
      cout << "\t" << "Type: " << get_feature_name(feature.type()) << endl;

      cout << "\t" << " The following subfeatures are presented:" << endl;
      for (auto [subfeature_name, subfeature] : feature.subfeatures()) {
        cout << "\t\t" << "Name: " << subfeature.name() << endl;
        cout << "\t\t" << "Number: " << subfeature.number() << endl;
        cout << "\t\t" << "Type: " << get_subfeature_name(subfeature.type()) << endl;
        cout << "\t\t" << "Value: " << subfeature.value() << endl;
        cout << "\t\t" << "CM/R/W: " << subfeature.is_compute_mapped() << "/" << subfeature.is_readable() << "/" << subfeature.is_writable() << endl;
        cout << endl;
      }
    }

    cout << endl;
  }

  return 0;
}