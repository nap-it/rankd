#ifndef RANK_PRELUDE_TIMEOUT_TABLE_H
#define RANK_PRELUDE_TIMEOUT_TABLE_H

#include <map>

#include "structs/identifier.h"
#include "structs/timeout_handler_spec.h"

typedef std::map<UUIDv4, TimeoutHandlerSpec> TimeoutTable;

#endif  // RANK_PRELUDE_TIMEOUT_TABLE_H
