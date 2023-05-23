// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file localnet.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _FAST_DDS_GENERATED_LOCALNET_LOCALNET_H_
#define _FAST_DDS_GENERATED_LOCALNET_LOCALNET_H_


#include <fastrtps/utils/fixed_size_string.hpp>

#include <stdint.h>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define eProsima_user_DllExport
#endif  // _WIN32

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(localnet_SOURCE)
#define localnet_DllAPI __declspec( dllexport )
#else
#define localnet_DllAPI __declspec( dllimport )
#endif // localnet_SOURCE
#else
#define localnet_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define localnet_DllAPI
#endif // _WIN32

namespace eprosima {
namespace fastcdr {
class Cdr;
} // namespace fastcdr
} // namespace eprosima


namespace Localnet {
    /*!
     * @brief This class represents the structure NetworkInterface defined by the user in the IDL file.
     * @ingroup LOCALNET
     */
    class NetworkInterface
    {
    public:

        /*!
         * @brief Default constructor.
         */
        eProsima_user_DllExport NetworkInterface();

        /*!
         * @brief Default destructor.
         */
        eProsima_user_DllExport ~NetworkInterface();

        /*!
         * @brief Copy constructor.
         * @param x Reference to the object Localnet::NetworkInterface that will be copied.
         */
        eProsima_user_DllExport NetworkInterface(
                const NetworkInterface& x);

        /*!
         * @brief Move constructor.
         * @param x Reference to the object Localnet::NetworkInterface that will be copied.
         */
        eProsima_user_DllExport NetworkInterface(
                NetworkInterface&& x) noexcept;

        /*!
         * @brief Copy assignment.
         * @param x Reference to the object Localnet::NetworkInterface that will be copied.
         */
        eProsima_user_DllExport NetworkInterface& operator =(
                const NetworkInterface& x);

        /*!
         * @brief Move assignment.
         * @param x Reference to the object Localnet::NetworkInterface that will be copied.
         */
        eProsima_user_DllExport NetworkInterface& operator =(
                NetworkInterface&& x) noexcept;

        /*!
         * @brief Comparison operator.
         * @param x Localnet::NetworkInterface object to compare.
         */
        eProsima_user_DllExport bool operator ==(
                const NetworkInterface& x) const;

        /*!
         * @brief Comparison operator.
         * @param x Localnet::NetworkInterface object to compare.
         */
        eProsima_user_DllExport bool operator !=(
                const NetworkInterface& x) const;

        /*!
         * @brief This function copies the value in member mac_address
         * @param _mac_address New value to be copied in member mac_address
         */
        eProsima_user_DllExport void mac_address(
                const std::string& _mac_address);

        /*!
         * @brief This function moves the value in member mac_address
         * @param _mac_address New value to be moved in member mac_address
         */
        eProsima_user_DllExport void mac_address(
                std::string&& _mac_address);

        /*!
         * @brief This function returns a constant reference to member mac_address
         * @return Constant reference to member mac_address
         */
        eProsima_user_DllExport const std::string& mac_address() const;

        /*!
         * @brief This function returns a reference to member mac_address
         * @return Reference to member mac_address
         */
        eProsima_user_DllExport std::string& mac_address();
        /*!
         * @brief This function copies the value in member ip4_addresses
         * @param _ip4_addresses New value to be copied in member ip4_addresses
         */
        eProsima_user_DllExport void ip4_addresses(
                const std::vector<std::string>& _ip4_addresses);

        /*!
         * @brief This function moves the value in member ip4_addresses
         * @param _ip4_addresses New value to be moved in member ip4_addresses
         */
        eProsima_user_DllExport void ip4_addresses(
                std::vector<std::string>&& _ip4_addresses);

        /*!
         * @brief This function returns a constant reference to member ip4_addresses
         * @return Constant reference to member ip4_addresses
         */
        eProsima_user_DllExport const std::vector<std::string>& ip4_addresses() const;

        /*!
         * @brief This function returns a reference to member ip4_addresses
         * @return Reference to member ip4_addresses
         */
        eProsima_user_DllExport std::vector<std::string>& ip4_addresses();
        /*!
         * @brief This function copies the value in member ip6_addresses
         * @param _ip6_addresses New value to be copied in member ip6_addresses
         */
        eProsima_user_DllExport void ip6_addresses(
                const std::vector<std::string>& _ip6_addresses);

        /*!
         * @brief This function moves the value in member ip6_addresses
         * @param _ip6_addresses New value to be moved in member ip6_addresses
         */
        eProsima_user_DllExport void ip6_addresses(
                std::vector<std::string>&& _ip6_addresses);

        /*!
         * @brief This function returns a constant reference to member ip6_addresses
         * @return Constant reference to member ip6_addresses
         */
        eProsima_user_DllExport const std::vector<std::string>& ip6_addresses() const;

        /*!
         * @brief This function returns a reference to member ip6_addresses
         * @return Reference to member ip6_addresses
         */
        eProsima_user_DllExport std::vector<std::string>& ip6_addresses();
        /*!
         * @brief This function sets a value in member has_carrier
         * @param _has_carrier New value for member has_carrier
         */
        eProsima_user_DllExport void has_carrier(
                bool _has_carrier);

        /*!
         * @brief This function returns the value of member has_carrier
         * @return Value of member has_carrier
         */
        eProsima_user_DllExport bool has_carrier() const;

        /*!
         * @brief This function returns a reference to member has_carrier
         * @return Reference to member has_carrier
         */
        eProsima_user_DllExport bool& has_carrier();

        /*!
         * @brief This function copies the value in member duplex_type
         * @param _duplex_type New value to be copied in member duplex_type
         */
        eProsima_user_DllExport void duplex_type(
                const std::string& _duplex_type);

        /*!
         * @brief This function moves the value in member duplex_type
         * @param _duplex_type New value to be moved in member duplex_type
         */
        eProsima_user_DllExport void duplex_type(
                std::string&& _duplex_type);

        /*!
         * @brief This function returns a constant reference to member duplex_type
         * @return Constant reference to member duplex_type
         */
        eProsima_user_DllExport const std::string& duplex_type() const;

        /*!
         * @brief This function returns a reference to member duplex_type
         * @return Reference to member duplex_type
         */
        eProsima_user_DllExport std::string& duplex_type();
        /*!
         * @brief This function sets a value in member interface_index
         * @param _interface_index New value for member interface_index
         */
        eProsima_user_DllExport void interface_index(
                uint32_t _interface_index);

        /*!
         * @brief This function returns the value of member interface_index
         * @return Value of member interface_index
         */
        eProsima_user_DllExport uint32_t interface_index() const;

        /*!
         * @brief This function returns a reference to member interface_index
         * @return Reference to member interface_index
         */
        eProsima_user_DllExport uint32_t& interface_index();

        /*!
         * @brief This function sets a value in member enslaved_interface_index
         * @param _enslaved_interface_index New value for member enslaved_interface_index
         */
        eProsima_user_DllExport void enslaved_interface_index(
                uint32_t _enslaved_interface_index);

        /*!
         * @brief This function returns the value of member enslaved_interface_index
         * @return Value of member enslaved_interface_index
         */
        eProsima_user_DllExport uint32_t enslaved_interface_index() const;

        /*!
         * @brief This function returns a reference to member enslaved_interface_index
         * @return Reference to member enslaved_interface_index
         */
        eProsima_user_DllExport uint32_t& enslaved_interface_index();

        /*!
         * @brief This function sets a value in member mtu
         * @param _mtu New value for member mtu
         */
        eProsima_user_DllExport void mtu(
                uint32_t _mtu);

        /*!
         * @brief This function returns the value of member mtu
         * @return Value of member mtu
         */
        eProsima_user_DllExport uint32_t mtu() const;

        /*!
         * @brief This function returns a reference to member mtu
         * @return Reference to member mtu
         */
        eProsima_user_DllExport uint32_t& mtu();

        /*!
         * @brief This function copies the value in member operation_state
         * @param _operation_state New value to be copied in member operation_state
         */
        eProsima_user_DllExport void operation_state(
                const std::string& _operation_state);

        /*!
         * @brief This function moves the value in member operation_state
         * @param _operation_state New value to be moved in member operation_state
         */
        eProsima_user_DllExport void operation_state(
                std::string&& _operation_state);

        /*!
         * @brief This function returns a constant reference to member operation_state
         * @return Constant reference to member operation_state
         */
        eProsima_user_DllExport const std::string& operation_state() const;

        /*!
         * @brief This function returns a reference to member operation_state
         * @return Reference to member operation_state
         */
        eProsima_user_DllExport std::string& operation_state();
        /*!
         * @brief This function sets a value in member speed
         * @param _speed New value for member speed
         */
        eProsima_user_DllExport void speed(
                int64_t _speed);

        /*!
         * @brief This function returns the value of member speed
         * @return Value of member speed
         */
        eProsima_user_DllExport int64_t speed() const;

        /*!
         * @brief This function returns a reference to member speed
         * @return Reference to member speed
         */
        eProsima_user_DllExport int64_t& speed();

        /*!
         * @brief This function sets a value in member rx_bytes
         * @param _rx_bytes New value for member rx_bytes
         */
        eProsima_user_DllExport void rx_bytes(
                int64_t _rx_bytes);

        /*!
         * @brief This function returns the value of member rx_bytes
         * @return Value of member rx_bytes
         */
        eProsima_user_DllExport int64_t rx_bytes() const;

        /*!
         * @brief This function returns a reference to member rx_bytes
         * @return Reference to member rx_bytes
         */
        eProsima_user_DllExport int64_t& rx_bytes();

        /*!
         * @brief This function sets a value in member tx_bytes
         * @param _tx_bytes New value for member tx_bytes
         */
        eProsima_user_DllExport void tx_bytes(
                int64_t _tx_bytes);

        /*!
         * @brief This function returns the value of member tx_bytes
         * @return Value of member tx_bytes
         */
        eProsima_user_DllExport int64_t tx_bytes() const;

        /*!
         * @brief This function returns a reference to member tx_bytes
         * @return Reference to member tx_bytes
         */
        eProsima_user_DllExport int64_t& tx_bytes();

        /*!
         * @brief This function sets a value in member rx_packets
         * @param _rx_packets New value for member rx_packets
         */
        eProsima_user_DllExport void rx_packets(
                int64_t _rx_packets);

        /*!
         * @brief This function returns the value of member rx_packets
         * @return Value of member rx_packets
         */
        eProsima_user_DllExport int64_t rx_packets() const;

        /*!
         * @brief This function returns a reference to member rx_packets
         * @return Reference to member rx_packets
         */
        eProsima_user_DllExport int64_t& rx_packets();

        /*!
         * @brief This function sets a value in member tx_packets
         * @param _tx_packets New value for member tx_packets
         */
        eProsima_user_DllExport void tx_packets(
                int64_t _tx_packets);

        /*!
         * @brief This function returns the value of member tx_packets
         * @return Value of member tx_packets
         */
        eProsima_user_DllExport int64_t tx_packets() const;

        /*!
         * @brief This function returns a reference to member tx_packets
         * @return Reference to member tx_packets
         */
        eProsima_user_DllExport int64_t& tx_packets();

        /*!
         * @brief This function sets a value in member rx_errors
         * @param _rx_errors New value for member rx_errors
         */
        eProsima_user_DllExport void rx_errors(
                uint32_t _rx_errors);

        /*!
         * @brief This function returns the value of member rx_errors
         * @return Value of member rx_errors
         */
        eProsima_user_DllExport uint32_t rx_errors() const;

        /*!
         * @brief This function returns a reference to member rx_errors
         * @return Reference to member rx_errors
         */
        eProsima_user_DllExport uint32_t& rx_errors();

        /*!
         * @brief This function sets a value in member tx_errors
         * @param _tx_errors New value for member tx_errors
         */
        eProsima_user_DllExport void tx_errors(
                uint32_t _tx_errors);

        /*!
         * @brief This function returns the value of member tx_errors
         * @return Value of member tx_errors
         */
        eProsima_user_DllExport uint32_t tx_errors() const;

        /*!
         * @brief This function returns a reference to member tx_errors
         * @return Reference to member tx_errors
         */
        eProsima_user_DllExport uint32_t& tx_errors();

        /*!
         * @brief This function sets a value in member rx_dropped
         * @param _rx_dropped New value for member rx_dropped
         */
        eProsima_user_DllExport void rx_dropped(
                uint32_t _rx_dropped);

        /*!
         * @brief This function returns the value of member rx_dropped
         * @return Value of member rx_dropped
         */
        eProsima_user_DllExport uint32_t rx_dropped() const;

        /*!
         * @brief This function returns a reference to member rx_dropped
         * @return Reference to member rx_dropped
         */
        eProsima_user_DllExport uint32_t& rx_dropped();

        /*!
         * @brief This function sets a value in member tx_dropped
         * @param _tx_dropped New value for member tx_dropped
         */
        eProsima_user_DllExport void tx_dropped(
                uint32_t _tx_dropped);

        /*!
         * @brief This function returns the value of member tx_dropped
         * @return Value of member tx_dropped
         */
        eProsima_user_DllExport uint32_t tx_dropped() const;

        /*!
         * @brief This function returns a reference to member tx_dropped
         * @return Reference to member tx_dropped
         */
        eProsima_user_DllExport uint32_t& tx_dropped();


        /*!
        * @brief This function returns the maximum serialized size of an object
        * depending on the buffer alignment.
        * @param current_alignment Buffer alignment.
        * @return Maximum serialized size.
        */
        eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function returns the serialized size of a data depending on the buffer alignment.
         * @param data Data which is calculated its serialized size.
         * @param current_alignment Buffer alignment.
         * @return Serialized size.
         */
        eProsima_user_DllExport static size_t getCdrSerializedSize(
                const Localnet::NetworkInterface& data,
                size_t current_alignment = 0);


        /*!
         * @brief This function serializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serialize(
                eprosima::fastcdr::Cdr& cdr) const;

        /*!
         * @brief This function deserializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void deserialize(
                eprosima::fastcdr::Cdr& cdr);



        /*!
         * @brief This function returns the maximum serialized size of the Key of an object
         * depending on the buffer alignment.
         * @param current_alignment Buffer alignment.
         * @return Maximum serialized size.
         */
        eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function tells you if the Key has been defined for this type
         */
        eProsima_user_DllExport static bool isKeyDefined();

        /*!
         * @brief This function serializes the key members of an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serializeKey(
                eprosima::fastcdr::Cdr& cdr) const;

    private:

        std::string m_mac_address;
        std::vector<std::string> m_ip4_addresses;
        std::vector<std::string> m_ip6_addresses;
        bool m_has_carrier;
        std::string m_duplex_type;
        uint32_t m_interface_index;
        uint32_t m_enslaved_interface_index;
        uint32_t m_mtu;
        std::string m_operation_state;
        int64_t m_speed;
        int64_t m_rx_bytes;
        int64_t m_tx_bytes;
        int64_t m_rx_packets;
        int64_t m_tx_packets;
        uint32_t m_rx_errors;
        uint32_t m_tx_errors;
        uint32_t m_rx_dropped;
        uint32_t m_tx_dropped;

    };
    /*!
     * @brief This class represents the structure MapEntry defined by the user in the IDL file.
     * @ingroup LOCALNET
     */
    class MapEntry
    {
    public:

        /*!
         * @brief Default constructor.
         */
        eProsima_user_DllExport MapEntry();

        /*!
         * @brief Default destructor.
         */
        eProsima_user_DllExport ~MapEntry();

        /*!
         * @brief Copy constructor.
         * @param x Reference to the object Localnet::MapEntry that will be copied.
         */
        eProsima_user_DllExport MapEntry(
                const MapEntry& x);

        /*!
         * @brief Move constructor.
         * @param x Reference to the object Localnet::MapEntry that will be copied.
         */
        eProsima_user_DllExport MapEntry(
                MapEntry&& x) noexcept;

        /*!
         * @brief Copy assignment.
         * @param x Reference to the object Localnet::MapEntry that will be copied.
         */
        eProsima_user_DllExport MapEntry& operator =(
                const MapEntry& x);

        /*!
         * @brief Move assignment.
         * @param x Reference to the object Localnet::MapEntry that will be copied.
         */
        eProsima_user_DllExport MapEntry& operator =(
                MapEntry&& x) noexcept;

        /*!
         * @brief Comparison operator.
         * @param x Localnet::MapEntry object to compare.
         */
        eProsima_user_DllExport bool operator ==(
                const MapEntry& x) const;

        /*!
         * @brief Comparison operator.
         * @param x Localnet::MapEntry object to compare.
         */
        eProsima_user_DllExport bool operator !=(
                const MapEntry& x) const;

        /*!
         * @brief This function copies the value in member interface_name
         * @param _interface_name New value to be copied in member interface_name
         */
        eProsima_user_DllExport void interface_name(
                const std::string& _interface_name);

        /*!
         * @brief This function moves the value in member interface_name
         * @param _interface_name New value to be moved in member interface_name
         */
        eProsima_user_DllExport void interface_name(
                std::string&& _interface_name);

        /*!
         * @brief This function returns a constant reference to member interface_name
         * @return Constant reference to member interface_name
         */
        eProsima_user_DllExport const std::string& interface_name() const;

        /*!
         * @brief This function returns a reference to member interface_name
         * @return Reference to member interface_name
         */
        eProsima_user_DllExport std::string& interface_name();
        /*!
         * @brief This function copies the value in member interface_item
         * @param _interface_item New value to be copied in member interface_item
         */
        eProsima_user_DllExport void interface_item(
                const Localnet::NetworkInterface& _interface_item);

        /*!
         * @brief This function moves the value in member interface_item
         * @param _interface_item New value to be moved in member interface_item
         */
        eProsima_user_DllExport void interface_item(
                Localnet::NetworkInterface&& _interface_item);

        /*!
         * @brief This function returns a constant reference to member interface_item
         * @return Constant reference to member interface_item
         */
        eProsima_user_DllExport const Localnet::NetworkInterface& interface_item() const;

        /*!
         * @brief This function returns a reference to member interface_item
         * @return Reference to member interface_item
         */
        eProsima_user_DllExport Localnet::NetworkInterface& interface_item();

        /*!
        * @brief This function returns the maximum serialized size of an object
        * depending on the buffer alignment.
        * @param current_alignment Buffer alignment.
        * @return Maximum serialized size.
        */
        eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function returns the serialized size of a data depending on the buffer alignment.
         * @param data Data which is calculated its serialized size.
         * @param current_alignment Buffer alignment.
         * @return Serialized size.
         */
        eProsima_user_DllExport static size_t getCdrSerializedSize(
                const Localnet::MapEntry& data,
                size_t current_alignment = 0);


        /*!
         * @brief This function serializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serialize(
                eprosima::fastcdr::Cdr& cdr) const;

        /*!
         * @brief This function deserializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void deserialize(
                eprosima::fastcdr::Cdr& cdr);



        /*!
         * @brief This function returns the maximum serialized size of the Key of an object
         * depending on the buffer alignment.
         * @param current_alignment Buffer alignment.
         * @return Maximum serialized size.
         */
        eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function tells you if the Key has been defined for this type
         */
        eProsima_user_DllExport static bool isKeyDefined();

        /*!
         * @brief This function serializes the key members of an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serializeKey(
                eprosima::fastcdr::Cdr& cdr) const;

    private:

        std::string m_interface_name;
        Localnet::NetworkInterface m_interface_item;

    };
    /*!
     * @brief This class represents the structure LocalnetStatus defined by the user in the IDL file.
     * @ingroup LOCALNET
     */
    class LocalnetStatus
    {
    public:

        /*!
         * @brief Default constructor.
         */
        eProsima_user_DllExport LocalnetStatus();

        /*!
         * @brief Default destructor.
         */
        eProsima_user_DllExport ~LocalnetStatus();

        /*!
         * @brief Copy constructor.
         * @param x Reference to the object Localnet::LocalnetStatus that will be copied.
         */
        eProsima_user_DllExport LocalnetStatus(
                const LocalnetStatus& x);

        /*!
         * @brief Move constructor.
         * @param x Reference to the object Localnet::LocalnetStatus that will be copied.
         */
        eProsima_user_DllExport LocalnetStatus(
                LocalnetStatus&& x) noexcept;

        /*!
         * @brief Copy assignment.
         * @param x Reference to the object Localnet::LocalnetStatus that will be copied.
         */
        eProsima_user_DllExport LocalnetStatus& operator =(
                const LocalnetStatus& x);

        /*!
         * @brief Move assignment.
         * @param x Reference to the object Localnet::LocalnetStatus that will be copied.
         */
        eProsima_user_DllExport LocalnetStatus& operator =(
                LocalnetStatus&& x) noexcept;

        /*!
         * @brief Comparison operator.
         * @param x Localnet::LocalnetStatus object to compare.
         */
        eProsima_user_DllExport bool operator ==(
                const LocalnetStatus& x) const;

        /*!
         * @brief Comparison operator.
         * @param x Localnet::LocalnetStatus object to compare.
         */
        eProsima_user_DllExport bool operator !=(
                const LocalnetStatus& x) const;

        /*!
         * @brief This function sets a value in member timestamp
         * @param _timestamp New value for member timestamp
         */
        eProsima_user_DllExport void timestamp(
                uint64_t _timestamp);

        /*!
         * @brief This function returns the value of member timestamp
         * @return Value of member timestamp
         */
        eProsima_user_DllExport uint64_t timestamp() const;

        /*!
         * @brief This function returns a reference to member timestamp
         * @return Reference to member timestamp
         */
        eProsima_user_DllExport uint64_t& timestamp();

        /*!
         * @brief This function copies the value in member interfaces
         * @param _interfaces New value to be copied in member interfaces
         */
        eProsima_user_DllExport void interfaces(
                const std::vector<Localnet::MapEntry>& _interfaces);

        /*!
         * @brief This function moves the value in member interfaces
         * @param _interfaces New value to be moved in member interfaces
         */
        eProsima_user_DllExport void interfaces(
                std::vector<Localnet::MapEntry>&& _interfaces);

        /*!
         * @brief This function returns a constant reference to member interfaces
         * @return Constant reference to member interfaces
         */
        eProsima_user_DllExport const std::vector<Localnet::MapEntry>& interfaces() const;

        /*!
         * @brief This function returns a reference to member interfaces
         * @return Reference to member interfaces
         */
        eProsima_user_DllExport std::vector<Localnet::MapEntry>& interfaces();

        /*!
        * @brief This function returns the maximum serialized size of an object
        * depending on the buffer alignment.
        * @param current_alignment Buffer alignment.
        * @return Maximum serialized size.
        */
        eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function returns the serialized size of a data depending on the buffer alignment.
         * @param data Data which is calculated its serialized size.
         * @param current_alignment Buffer alignment.
         * @return Serialized size.
         */
        eProsima_user_DllExport static size_t getCdrSerializedSize(
                const Localnet::LocalnetStatus& data,
                size_t current_alignment = 0);


        /*!
         * @brief This function serializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serialize(
                eprosima::fastcdr::Cdr& cdr) const;

        /*!
         * @brief This function deserializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void deserialize(
                eprosima::fastcdr::Cdr& cdr);



        /*!
         * @brief This function returns the maximum serialized size of the Key of an object
         * depending on the buffer alignment.
         * @param current_alignment Buffer alignment.
         * @return Maximum serialized size.
         */
        eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function tells you if the Key has been defined for this type
         */
        eProsima_user_DllExport static bool isKeyDefined();

        /*!
         * @brief This function serializes the key members of an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serializeKey(
                eprosima::fastcdr::Cdr& cdr) const;

    private:

        uint64_t m_timestamp;
        std::vector<Localnet::MapEntry> m_interfaces;

    };
} // namespace Localnet

#endif // _FAST_DDS_GENERATED_LOCALNET_LOCALNET_H_