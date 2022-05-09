#include <iostream>

#include <sys/sysinfo.h>

using namespace std;

int main() {
  struct sysinfo system_info;

  int error_code = sysinfo(&system_info);

  cout << "The system is up by " << system_info.uptime << " seconds." << endl;

  return 0;
}