#ifndef RANK_PRELUDE_DISPATCHER_ALL_H
#define RANK_PRELUDE_DISPATCHER_ALL_H

#ifndef FROM_SIMUZILLA
#include "structs/dispatchers/receiver_dds.h"
#include "structs/dispatchers/receiver_l2.h"
#include "structs/dispatchers/receiver_l3.h"
#include "structs/dispatchers/receiver_simulation.h"
#include "structs/dispatchers/sender.h"
#else
#include "structs/dispatchers/receiver_simulation.h"
#include "structs/dispatchers/sender.h"
#endif

#endif //RANK_PRELUDE_DISPATCHER_ALL_H
