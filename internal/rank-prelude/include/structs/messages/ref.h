#ifndef RANK_PRELUDE_REF_H
#define RANK_PRELUDE_REF_H

#include "structs/message.h"
#include "structs/messages/header.h"

class REF : public Message {
public:
    // Instance handlers.
    REF(const Header& header) : Message(header) {
    }
    REF(const Header& header, const char* marshalled_data) : Message(header) {
    }
};


#endif  // RANK_PRELUDE_REF_H
