#ifndef RANK_PRELUDE_EAR_H
#define RANK_PRELUDE_EAR_H

#include <array>
#include <cstdint>
#include <vector>

#include "structs/message.h"
#include "constants.h"
#include "structs/messages/header.h"
#include "structs/requesting_capabilities.h"

#include "utils/marshalling.h"
#include "utils/messaging.h"

class EAR : public Message {
public:
    // Instance handlers.
    EAR(const UUIDv4& uuid, uint8_t priority, uint8_t listener_length, const std::array<uint8_t, RANK_LISTENER_MAX_LEN>& listener, uint16_t payload_length,
        const std::vector<uint8_t>& payload) :
        Message(Header(RANK_HEADER_VERSION, MessageType::EAR, uuid)), _priority {priority}, _listener_length {listener_length}, _listener {listener},
        _payload_length {payload_length}, _payload {payload}, _reserved{0} {
    }
    EAR(const Header& header, uint8_t priority, uint8_t listener_length, const std::array<uint8_t, RANK_LISTENER_MAX_LEN>& listener, uint16_t payload_length,
        const std::vector<uint8_t>& payload) :
        Message(header), _priority {priority}, _listener_length {listener_length}, _listener {listener},
        _payload_length {payload_length}, _payload {payload}, _reserved{0} {
    }
    EAR(const Header& header, const std::vector<uint8_t>& marshalled_data) : Message(header) {
        _priority = marshalled_data.at(0) >> 5 & 0x07;
        _listener_length = marshalled_data.at(0) >> 2 & 0x07;
        _reserved = 0;

        switch (_listener_length) {
            case RANK_MAR_MESSAGE_LEN_LT_CODE_0:
                _listener.at(0) = marshalled_data.at(1);
                _payload_length = marshalled_data.at(1+SIMUZILLA_ADDR_LEN) << 8 | marshalled_data.at(1+SIMUZILLA_ADDR_LEN+1);
                for (int i = 0; i != _payload_length; i++) {
                    _payload.push_back(marshalled_data.at(1+SIMUZILLA_ADDR_LEN+2+i));
                }
                break;
            case RANK_EAR_MESSAGE_LEN_LT_IP4:
                for (int i = 0; i != IPV4_ADDR_LEN; i++) {
                    _listener.at(i) = marshalled_data.at(i+1);
                }
                _payload_length = marshalled_data.at(1+IPV4_ADDR_LEN) << 8 | marshalled_data.at(1+IPV4_ADDR_LEN+1);
                for (int i = 0; i != _payload_length; i++) {
                    _payload.push_back(marshalled_data.at(1+IPV4_ADDR_LEN+2+i));
                }
                break;
            case RANK_EAR_MESSAGE_LEN_LT_MAC:
                for (int i = 0; i != MAC_ADDR_LEN; i++) {
                    _listener.at(i) = marshalled_data.at(i+1);
                }
                _payload_length = marshalled_data.at(1+MAC_ADDR_LEN) << 8 | marshalled_data.at(1+MAC_ADDR_LEN+1);
                for (int i = 0; i != _payload_length; i++) {
                    _payload.push_back(marshalled_data.at(1+MAC_ADDR_LEN+2+i));
                }
                break;
            case RANK_EAR_MESSAGE_LEN_LT_IP6:
                for (int i = 0; i != IPV6_ADDR_LEN; i++) {
                    _listener.at(i) = marshalled_data.at(i+1);
                }
                _payload_length = marshalled_data.at(1+IPV6_ADDR_LEN) << 8 | marshalled_data.at(1+IPV6_ADDR_LEN+1);
                for (int i = 0; i != _payload_length; i++) {
                    _payload.push_back(marshalled_data.at(1+IPV6_ADDR_LEN+2+i));
                }
                break;
            case RANK_EAR_MESSAGE_LEN_LT_DDS:
                for (int i = 0; i != DDS_ADDR_LEN; i++) {
                    _listener.at(i) = marshalled_data.at(i+1);
                }
                _payload_length = marshalled_data.at(1+DDS_ADDR_LEN) << 8 | marshalled_data.at(1+DDS_ADDR_LEN+1);
                for (int i = 0; i != _payload_length; i++) {
                    _payload.push_back(marshalled_data.at(1+DDS_ADDR_LEN+2+i));
                }
                break;
            default:
                throw std::exception(); // TODO Handle this case.
        }


    }

    // Marshalling features.
    RequestingCapabilities requirements() const;

    // Parsing tools.
    uint8_t expand_listener_length(uint8_t length) const;

    // Getters.
    uint8_t priority() const;
    uint8_t listener_length() const;
    uint8_t reserved() const;
    std::vector<uint8_t> listener() const;
    uint16_t payload_length() const;
    std::vector<uint8_t> payload() const;

    // Derived member methods.
    const std::vector<uint8_t> raw_payload() const override;

    // Destructor.
    ~EAR();

private:
    uint8_t _priority;
    uint8_t _listener_length;
    uint8_t _reserved;
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> _listener;
    uint16_t _payload_length;
    std::vector<uint8_t> _payload;
};


#endif  // RANK_PRELUDE_EAR_H
