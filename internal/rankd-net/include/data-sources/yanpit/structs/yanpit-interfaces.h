#ifndef RANKD_YANPIT_INTERFACES_H
#define RANKD_YANPIT_INTERFACES_H

#include <map>
#include <string>
#include <vector>

// RapidJSON inclusions.
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"

class YanpitNetworkInterface {
public:
    YanpitNetworkInterface(const std::string& name) {
        _name = name;
    }
    const std::string& name() const {
        return _name;
    }
    void name(const std::string& name) {
        _name = name;
    }
    const std::string& address() const {
        return _address;
    }
    void address(const std::string& address) {
        _address = address;
    }
    const std::vector<std::string>& ip_4_addresses() const {
        return _ip4_addresses;
    }
    void ip_4_addresses(const std::vector<std::string>& ip4Addresses) {
        _ip4_addresses = ip4Addresses;
    }
    const std::vector<std::string>& ip_6_addresses() const {
        return _ip6_addresses;
    }
    void ip_6_addresses(const std::vector<std::string>& ip6Addresses) {
        _ip6_addresses = ip6Addresses;
    }
    bool is_carrier() const {
        return _carrier;
    }
    void carrier(bool carrier) {
        _carrier = carrier;
    }
    const std::string& duplex_type() const {
        return _duplex_type;
    }
    void duplex_type(const std::string& duplexType) {
        _duplex_type = duplexType;
    }
    unsigned int interface_index() const {
        return _interface_index;
    }
    void interface_index(unsigned int interfaceIndex) {
        _interface_index = interfaceIndex;
    }
    unsigned int enslaved_interface_index() const {
        return _enslaved_interface_index;
    }
    void enslaved_interface_index(unsigned int enslavedInterfaceIndex) {
        _enslaved_interface_index = enslavedInterfaceIndex;
    }
    unsigned int mtu() const {
        return _mtu;
    }
    void mtu(unsigned int mtu) {
        _mtu = mtu;
    }
    const std::string& operation_state() const {
        return _operation_state;
    }
    void operation_state(const std::string& operationState) {
        _operation_state = operationState;
    }
    unsigned int speed() const {
        return _speed;
    }
    void speed(unsigned int speed) {
        _speed = speed;
    }
    unsigned int rx_bytes() const {
        return _rx_bytes;
    }
    void rx_bytes(unsigned int rxBytes) {
        _rx_bytes = rxBytes;
    }
    unsigned int tx_bytes() const {
        return _tx_bytes;
    }
    void tx_bytes(unsigned int txBytes) {
        _tx_bytes = txBytes;
    }
    unsigned int rx_packets() const {
        return _rx_packets;
    }
    void rx_packets(unsigned int rxPackets) {
        _rx_packets = rxPackets;
    }
    unsigned int tx_packets() const {
        return _tx_packets;
    }
    void tx_packets(unsigned int txPackets) {
        _tx_packets = txPackets;
    }
    unsigned int rx_errors() const {
        return _rx_errors;
    }
    void rx_errors(unsigned int rxErrors) {
        _rx_errors = rxErrors;
    }
    unsigned int tx_errors() const {
        return _tx_errors;
    }
    void tx_errors(unsigned int txErrors) {
        _tx_errors = txErrors;
    }
    unsigned int rx_dropped() const {
        return _rx_dropped;
    }
    void rx_dropped(unsigned int rxDropped) {
        _rx_dropped = rxDropped;
    }
    unsigned int tx_dropped() const {
        return _tx_dropped;
    }
    void tx_dropped(unsigned int txDropped) {
        _tx_dropped = txDropped;
    }
private:
    std::string _name {};
    std::string _address {};
    std::vector<std::string> _ip4_addresses {};
    std::vector<std::string> _ip6_addresses {};
    bool _carrier {};
    std::string _duplex_type {};
    unsigned int _interface_index {};
    unsigned int _enslaved_interface_index {};
    unsigned int _mtu {};
    std::string _operation_state {};
    unsigned int _speed {};
    unsigned int _rx_bytes {};
    unsigned int _tx_bytes {};
    unsigned int _rx_packets {};
    unsigned int _tx_packets {};
    unsigned int _rx_errors {};
    unsigned int _tx_errors {};
    unsigned int _rx_dropped {};
    unsigned int _tx_dropped {};
};


#endif  // RANKD_YANPIT_INTERFACES_H
