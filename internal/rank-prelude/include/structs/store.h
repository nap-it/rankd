#ifndef RANK_PRELUDE_STORE_H
#define RANK_PRELUDE_STORE_H

#include <map>
#include <set>

#include "structs/identifier.h"

class Handler;

typedef std::map<UUIDv4, Handler*> Store;

typedef std::map<UUIDv4, uint8_t> OriginSet;

#endif  // RANK_PRELUDE_STORE_H
