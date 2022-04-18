#ifndef RANKD_COMP_LIB_H
#define RANKD_COMP_LIB_H

#ifdef __APPLE__
#include "macos/cpu.h"
#elif __linux
#include "linux/cpu.h"
#endif

#endif // RANKD_COMP_LIB_H
