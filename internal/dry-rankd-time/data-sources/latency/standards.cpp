#include "data-sources/latency/standards.h"

std::string TAS::type_of_operation_name(const TypeOfOperation& type) {
    switch (type) {
        case TypeOfOperation::SetGateStates:
            return "set";
        case TypeOfOperation::SetAndHoldMac:
            return "set and hold";
        case TypeOfOperation::SetAndReleaseMac:
            return "set and release";
    }
}