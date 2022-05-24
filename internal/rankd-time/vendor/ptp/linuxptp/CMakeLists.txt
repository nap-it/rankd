cmake_minimum_required(VERSION 3.1.0)

project(linuxptp VERSION 3.1 LANGUAGES C)

file(GLOB ptp_src
        "config.h"
        "hash.h"
        "interface.h"
        "msg.h"
        "phc.h"
        "pmc_common.h"
        "print.h"
        "sk.h"
        "tlv.h"
        "raw.h"
        "transport.h"
        "udp.h"
        "udp6.h"
        "uds.h"
        "util.h"
        "config.c"
        "hash.c"
        "interface.c"
        "msg.c"
        "phc.c"
        "pmc_common.c"
        "print.c"
        "sk.c"
        "tlv.c"
        "raw.c"
        "transport.c"
        "udp.c"
        "udp6.c"
        "uds.c"
        "util.c"
)

add_library(linuxptp STATIC pmc.h ${ptp_src})
set_property(TARGET linuxptp PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(linuxptp m rt pthread)