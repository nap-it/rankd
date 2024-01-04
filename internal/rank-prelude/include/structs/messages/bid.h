#ifndef RANK_PRELUDE_BID_H
#define RANK_PRELUDE_BID_H

#include "structs/message.h"
#include "structs/messages/header.h"

class BID : public Message {
public:
    // Instance handlers.
    BID(const Header& header, float value) : Message(header), _value{value} {}
    BID(const Header& header, const char* marshalled_data) : Message(header) {}

    // Getters.
    float value() const;
private:
    float _value;
};


#endif  // RANK_PRELUDE_BID_H
