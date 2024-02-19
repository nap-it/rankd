cmake_minimum_required(VERSION 3.1.0)

project(linuxptp VERSION 3.1 LANGUAGES C)

file(GLOB pmc_src
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

if (APU)
    add_definitions("-DHAVE_ONESTEP_P2P")
endif()

if (DOCKER)
    add_definitions("-DHAVE_ONESTEP_P2P")
    add_definitions("-DHAVE_VCLOCKS")
endif()

add_library(pmc STATIC pmc.h ${pmc_src})
set_property(TARGET pmc PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(pmc m rt pthread)