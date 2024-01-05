#ifndef RANK_PRELUDE_STORE_H
#define RANK_PRELUDE_STORE_H

#include <map>

#include "structs/identifier.h"
#include "structs/handler.h"

typedef std::map<UUIDv4, Handler*> Store;

#endif  // RANK_PRELUDE_STORE_H
