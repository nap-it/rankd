#ifndef RANKD_COMP_LIB_H
#define RANKD_COMP_LIB_H

#ifdef __APPLE__
#include "macos/cpu.h"
#elif __linux
#include "linux/cpu.h"
#include "linux/memory.h"
#include "linux/os.h"
#include "linux/sensors.h"
#include "linux/storage.h"
#endif

#endif // RANKD_COMP_LIB_H
