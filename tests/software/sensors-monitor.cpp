#include <iostream>
#include <string>

//#include "comp-lib.h"
#include "sensors-c++/sensors.h"

using namespace std;

int main() {
  cout << "Hello Flag!" << endl;

  auto chips = sensors::get_detected_chips();
  cout << "vector size is " << chips.size() << "." << endl;

  for (auto& chip : chips) {
    cout << "Chip name: " << chip.name() << endl;
    cout << "Chip address: " << chip.address() << endl;
    cout << "Chip bus type (to int): " << (int)(chip.bus().type()) << endl;
    cout << "Chip bus adapter name: " << chip.bus().adapter_name() << endl;
    cout << "Chip bus nr: " << chip.bus().nr() << endl;
    cout << "Chip path: " << chip.path() << endl;
    cout << "Chip prefix: " << chip.prefix() << endl;
    cout << "  This chip has the following features:" << endl;
    for (auto& feature : chip.features()) {
      cout << "\t" << "Feature name: " << feature.name() << endl;
      cout << "\t" << "Feature type (as int): " << (int)feature.type() << endl;
      cout << "\t" << "Feature number: " << feature.number() << endl;
      cout << "\t" << "Feature label: " << feature.label() << endl;
      cout << "\t  " << "This feature has the following subfeatures: " << endl;
      for (auto& subfeature : feature.subfeatures()) {
        cout << "\t\t" << "Subfeature name: " << subfeature.name() << endl;
        cout << "\t\t" << "Subfeature number: " << subfeature.number() << endl;
        cout << "\t\t" << "Subfeature type (as int): " << (int)subfeature.type() << endl;
        cout << "\t\t" << "Subfeature compute mapping: " << subfeature.compute_mapping() << endl;
        cout << "\t\t" << "Subfeature read (double): " << subfeature.read() << endl;
        cout << "\t\t" << "Subfeature readable: " << subfeature.readable() << endl;
        cout << "\t\t" << "Subfeature writable: " << subfeature.writable() << endl;
      }

      //cout << feature.name() << ", with number " << feature.number() << ", label " << feature.label() << ", and the following subfeatures:" << endl << "\t";
      //for (auto& subfeature : feature.subfeatures()) {
      //  cout << subfeature.name() << ", whose value is of " << subfeature.number() << endl << "\t";
      //}
      //cout << endl << "\t";
    }
    cout << endl;
  }


  return 0;
}