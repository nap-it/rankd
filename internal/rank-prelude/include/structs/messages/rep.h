#ifndef RANK_PRELUDE_REP_H
#define RANK_PRELUDE_REP_H

#include <array>

#include <cstdint>

#include "structs/message.h"
#include "structs/messages/header.h"

class REP : public Message {
public:
    // Instance handlers.
    REP(const UUIDv4& uuid, uint8_t listener_length, const std::array<uint8_t, RANK_LISTENER_MAX_LEN>& listener) :
            Message(Header(RANK_HEADER_VERSION, MessageType::REP, uuid)), _listener_length {listener_length}, _listener {listener} {
    }
    REP(const UUIDv4& uuid, uint8_t listener_length, const std::vector<uint8_t>& listener) :
            Message(Header(RANK_HEADER_VERSION, MessageType::REP, uuid)), _listener_length {listener_length} {
        std::copy_n(listener.begin(), _listener_length, _listener.begin());
    }
    REP(const Header& header, uint8_t listener_length, const std::array<uint8_t, RANK_LISTENER_MAX_LEN>& listener) :
        Message(header), _listener_length {listener_length}, _listener {listener} {
    }
    REP(const Header& header, const std::vector<uint8_t>& marshalled_data) : Message(header) {
        _listener_length = marshalled_data.at(0) >> 5 & 0x07;
        _reserved = 0;

        switch (_listener_length) {
            case RANK_EAR_MESSAGE_LEN_LT_CODE_0:
                _listener.at(0) = marshalled_data.at(1);
                break;
            case RANK_EAR_MESSAGE_LEN_LT_IP4:
                for (int i = 0; i != IPV4_ADDR_LEN; i++) {
                    _listener.at(i) = marshalled_data.at(i+1);
                }
                break;
            case RANK_EAR_MESSAGE_LEN_LT_MAC:
                for (int i = 0; i != MAC_ADDR_LEN; i++) {
                    _listener.at(i) = marshalled_data.at(i+1);
                }
                break;
            case RANK_EAR_MESSAGE_LEN_LT_IP6:
                for (int i = 0; i != IPV6_ADDR_LEN; i++) {
                    _listener.at(i) = marshalled_data.at(i+1);
                }
                break;
            case RANK_EAR_MESSAGE_LEN_LT_DDS:
                for (int i = 0; i != DDS_ADDR_LEN; i++) {
                    _listener.at(i) = marshalled_data.at(i+1);
                }
                break;
            default:
                throw std::exception(); // TODO Handle this case.
        }
    }

    // Parsing tools.
    uint8_t expand_listener_length(uint8_t length) const;

    // Getters.
    uint8_t listener_length() const;
    uint8_t reserved() const;
    std::vector<uint8_t> listener() const;

    // Derived member methods.
    const std::vector<uint8_t> raw_payload() const override;
    std::string display() override;

    // Destructor.
    ~REP();

private:
    uint8_t _listener_length;
    uint8_t _reserved;
    std::array<uint8_t, RANK_LISTENER_MAX_LEN> _listener{};
};


#endif  // RANK_PRELUDE_REP_H
