#ifndef RANK_PRELUDE_STORE_H
#define RANK_PRELUDE_STORE_H

#include <map>

#include "structs/identifier.h"

class Handler;

typedef std::map<UUIDv4, Handler*> Store;

#endif  // RANK_PRELUDE_STORE_H
