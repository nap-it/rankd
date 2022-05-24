# Requirements of `rankd` project

In this document are detailed the requirements for both development and installation of this project's executables and libraries.

This project is mainly composed by three libraries placed in `pkg` and a set of internal tools for our main executable (the `rankd` executable) in the `internal` directory. External libraries are compiled or pre-compiled with the project as submodules where they are inherently needed, but pre-required when its usage its optional. One example of such a logic resides within the `pkg/rankd-time` library, as it is described in the section below.

## Requirements for `pkg/rankd-time`

This library, in order to function, requires a flag to be set in compile-time in order to pre-select some features available in the system, some of them exclusive. 

As this package is needed to request time-related information on a given TSN-capable board, two levels of features are required, which should be given as flags to the `cmake` program, when applicable. The flags are as follows:

* Per board kit vendor:
  * Set the flag `-DRELYUM` when the code is supposed to be ran in a Relyum-branded TSN-capable board.
* Per network monitoring software to be used:
  * Set the flag `-DSNMP` when this library is supposed to run an SNMP version for network monitoring;
  * Set the flag `-DNETCONF` when this library is supposed to run a NETCONF version for network monitoring.

**Note that** the setting of the NETCONF and SNMP simultaneously will result on only applying NETCONF to the program logic. No matter which option to use, libraries are needed in order to compile both options: 

* To use the SNMP client, please install beforehand the Net-SNMP library onto the machine, by consulting and download the latest version of the software in [the official Net-SNMP webpage](http://www.net-snmp.org/download.html). Following the download, you only need to execute the following commands, to install the library onto the system. 

  ```bash
  ./configure
  make -j$(nproc)
  sudo make install
  ```

* To use the NETCONF version, please install beforehand the `libyang` and `libssh` libraries first, as detailed below:
  
  ```bash
  # Install the libyang library.
  sudo apt install libpcre2-dev
  git clone https://github.com/CESNET/libyang.git
  cd libyang
  mkdir build
  cd build
  cmake ..
  make -j$(nproc)
  sudo make install
  
  # Install the libssh library.
  sudo apt install libssh2-1-dev
  ```
  