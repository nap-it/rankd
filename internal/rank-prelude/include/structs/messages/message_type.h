#ifndef RANK_PRELUDE_MESSAGE_TYPE_H
#define RANK_PRELUDE_MESSAGE_TYPE_H

#include <string>

enum class MessageType { NOTYPE = 7, EAR = 0, MAR = 1, BID = 2, ACC = 3, REF = 4, REP = 5 };

std::string message_type_to_string(const MessageType& type);

#endif  // RANK_PRELUDE_MESSAGE_TYPE_H
