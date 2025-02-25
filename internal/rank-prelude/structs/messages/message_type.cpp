#include "structs/messages/message_type.h"

std::string message_type_to_string(const MessageType& type) {
    switch (type) {
        case MessageType::NOTYPE:
            return "NOTYPE";
        case MessageType::EAR:
            return "EAR";
        case MessageType::MAR:
            return "MAR";
        case MessageType::BID:
            return "BID";
        case MessageType::ACC:
            return "ACC";
        case MessageType::REF:
            return "REF";
        case MessageType::REP:
            return "REP";
    }
}