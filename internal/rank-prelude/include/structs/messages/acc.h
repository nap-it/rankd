#ifndef RANK_PRELUDE_ACC_H
#define RANK_PRELUDE_ACC_H

#include "structs/message.h"
#include "structs/messages/header.h"

class ACC : public Message {
public:
    // Instance handlers.
    ACC(const Header& header) : Message(header) {
    }
    ACC(const Header& header, const char* marshalled_data) : Message(header) {
    }
};


#endif  // RANK_PRELUDE_ACC_H
