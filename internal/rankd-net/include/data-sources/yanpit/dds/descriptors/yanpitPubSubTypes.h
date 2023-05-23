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
 * @file yanpitPubSubTypes.h
 * This header file contains the declaration of the serialization functions.
 *
 * This file was generated by the tool fastcdrgen.
 */


#ifndef _FAST_DDS_GENERATED_YANPIT_YANPIT_PUBSUBTYPES_H_
#define _FAST_DDS_GENERATED_YANPIT_YANPIT_PUBSUBTYPES_H_

#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastrtps/utils/md5.h>

#include "yanpit.h"

#if !defined(GEN_API_VER) || (GEN_API_VER != 1)
#error \
    Generated yanpit is not compatible with current installed Fast DDS. Please, regenerate it with fastddsgen.
#endif  // GEN_API_VER

namespace Yanpit
{
    /*!
     * @brief This class represents the TopicDataType of the type IperfStatus defined by the user in the IDL file.
     * @ingroup YANPIT
     */
    class IperfStatusPubSubType : public eprosima::fastdds::dds::TopicDataType
    {
    public:

        typedef IperfStatus type;

        eProsima_user_DllExport IperfStatusPubSubType();

        eProsima_user_DllExport virtual ~IperfStatusPubSubType() override;

        eProsima_user_DllExport virtual bool serialize(
                void* data,
                eprosima::fastrtps::rtps::SerializedPayload_t* payload) override;

        eProsima_user_DllExport virtual bool deserialize(
                eprosima::fastrtps::rtps::SerializedPayload_t* payload,
                void* data) override;

        eProsima_user_DllExport virtual std::function<uint32_t()> getSerializedSizeProvider(
                void* data) override;

        eProsima_user_DllExport virtual bool getKey(
                void* data,
                eprosima::fastrtps::rtps::InstanceHandle_t* ihandle,
                bool force_md5 = false) override;

        eProsima_user_DllExport virtual void* createData() override;

        eProsima_user_DllExport virtual void deleteData(
                void* data) override;

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
        eProsima_user_DllExport inline bool is_bounded() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN
        eProsima_user_DllExport inline bool is_plain() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    #ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
        eProsima_user_DllExport inline bool construct_sample(
                void* memory) const override
        {
            new (memory) IperfStatus();
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

        MD5 m_md5;
        unsigned char* m_keyBuffer;
    };
    /*!
     * @brief This class represents the TopicDataType of the type ActiveTools defined by the user in the IDL file.
     * @ingroup YANPIT
     */
    class ActiveToolsPubSubType : public eprosima::fastdds::dds::TopicDataType
    {
    public:

        typedef ActiveTools type;

        eProsima_user_DllExport ActiveToolsPubSubType();

        eProsima_user_DllExport virtual ~ActiveToolsPubSubType() override;

        eProsima_user_DllExport virtual bool serialize(
                void* data,
                eprosima::fastrtps::rtps::SerializedPayload_t* payload) override;

        eProsima_user_DllExport virtual bool deserialize(
                eprosima::fastrtps::rtps::SerializedPayload_t* payload,
                void* data) override;

        eProsima_user_DllExport virtual std::function<uint32_t()> getSerializedSizeProvider(
                void* data) override;

        eProsima_user_DllExport virtual bool getKey(
                void* data,
                eprosima::fastrtps::rtps::InstanceHandle_t* ihandle,
                bool force_md5 = false) override;

        eProsima_user_DllExport virtual void* createData() override;

        eProsima_user_DllExport virtual void deleteData(
                void* data) override;

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
        eProsima_user_DllExport inline bool is_bounded() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN
        eProsima_user_DllExport inline bool is_plain() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    #ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
        eProsima_user_DllExport inline bool construct_sample(
                void* memory) const override
        {
            new (memory) ActiveTools();
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

        MD5 m_md5;
        unsigned char* m_keyBuffer;
    };
    /*!
     * @brief This class represents the TopicDataType of the type NcmStatus defined by the user in the IDL file.
     * @ingroup YANPIT
     */
    class NcmStatusPubSubType : public eprosima::fastdds::dds::TopicDataType
    {
    public:

        typedef NcmStatus type;

        eProsima_user_DllExport NcmStatusPubSubType();

        eProsima_user_DllExport virtual ~NcmStatusPubSubType() override;

        eProsima_user_DllExport virtual bool serialize(
                void* data,
                eprosima::fastrtps::rtps::SerializedPayload_t* payload) override;

        eProsima_user_DllExport virtual bool deserialize(
                eprosima::fastrtps::rtps::SerializedPayload_t* payload,
                void* data) override;

        eProsima_user_DllExport virtual std::function<uint32_t()> getSerializedSizeProvider(
                void* data) override;

        eProsima_user_DllExport virtual bool getKey(
                void* data,
                eprosima::fastrtps::rtps::InstanceHandle_t* ihandle,
                bool force_md5 = false) override;

        eProsima_user_DllExport virtual void* createData() override;

        eProsima_user_DllExport virtual void deleteData(
                void* data) override;

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
        eProsima_user_DllExport inline bool is_bounded() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN
        eProsima_user_DllExport inline bool is_plain() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    #ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
        eProsima_user_DllExport inline bool construct_sample(
                void* memory) const override
        {
            new (memory) NcmStatus();
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

        MD5 m_md5;
        unsigned char* m_keyBuffer;
    };
    /*!
     * @brief This class represents the TopicDataType of the type LocalnetStatus defined by the user in the IDL file.
     * @ingroup YANPIT
     */
    class LocalnetStatusPubSubType : public eprosima::fastdds::dds::TopicDataType
    {
    public:

        typedef LocalnetStatus type;

        eProsima_user_DllExport LocalnetStatusPubSubType();

        eProsima_user_DllExport virtual ~LocalnetStatusPubSubType() override;

        eProsima_user_DllExport virtual bool serialize(
                void* data,
                eprosima::fastrtps::rtps::SerializedPayload_t* payload) override;

        eProsima_user_DllExport virtual bool deserialize(
                eprosima::fastrtps::rtps::SerializedPayload_t* payload,
                void* data) override;

        eProsima_user_DllExport virtual std::function<uint32_t()> getSerializedSizeProvider(
                void* data) override;

        eProsima_user_DllExport virtual bool getKey(
                void* data,
                eprosima::fastrtps::rtps::InstanceHandle_t* ihandle,
                bool force_md5 = false) override;

        eProsima_user_DllExport virtual void* createData() override;

        eProsima_user_DllExport virtual void deleteData(
                void* data) override;

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
        eProsima_user_DllExport inline bool is_bounded() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN
        eProsima_user_DllExport inline bool is_plain() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    #ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
        eProsima_user_DllExport inline bool construct_sample(
                void* memory) const override
        {
            new (memory) LocalnetStatus();
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

        MD5 m_md5;
        unsigned char* m_keyBuffer;
    };
    /*!
     * @brief This class represents the TopicDataType of the type PassiveTools defined by the user in the IDL file.
     * @ingroup YANPIT
     */
    class PassiveToolsPubSubType : public eprosima::fastdds::dds::TopicDataType
    {
    public:

        typedef PassiveTools type;

        eProsima_user_DllExport PassiveToolsPubSubType();

        eProsima_user_DllExport virtual ~PassiveToolsPubSubType() override;

        eProsima_user_DllExport virtual bool serialize(
                void* data,
                eprosima::fastrtps::rtps::SerializedPayload_t* payload) override;

        eProsima_user_DllExport virtual bool deserialize(
                eprosima::fastrtps::rtps::SerializedPayload_t* payload,
                void* data) override;

        eProsima_user_DllExport virtual std::function<uint32_t()> getSerializedSizeProvider(
                void* data) override;

        eProsima_user_DllExport virtual bool getKey(
                void* data,
                eprosima::fastrtps::rtps::InstanceHandle_t* ihandle,
                bool force_md5 = false) override;

        eProsima_user_DllExport virtual void* createData() override;

        eProsima_user_DllExport virtual void deleteData(
                void* data) override;

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
        eProsima_user_DllExport inline bool is_bounded() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN
        eProsima_user_DllExport inline bool is_plain() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    #ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
        eProsima_user_DllExport inline bool construct_sample(
                void* memory) const override
        {
            new (memory) PassiveTools();
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

        MD5 m_md5;
        unsigned char* m_keyBuffer;
    };
    /*!
     * @brief This class represents the TopicDataType of the type YanpitStatus defined by the user in the IDL file.
     * @ingroup YANPIT
     */
    class YanpitStatusPubSubType : public eprosima::fastdds::dds::TopicDataType
    {
    public:

        typedef YanpitStatus type;

        eProsima_user_DllExport YanpitStatusPubSubType();

        eProsima_user_DllExport virtual ~YanpitStatusPubSubType() override;

        eProsima_user_DllExport virtual bool serialize(
                void* data,
                eprosima::fastrtps::rtps::SerializedPayload_t* payload) override;

        eProsima_user_DllExport virtual bool deserialize(
                eprosima::fastrtps::rtps::SerializedPayload_t* payload,
                void* data) override;

        eProsima_user_DllExport virtual std::function<uint32_t()> getSerializedSizeProvider(
                void* data) override;

        eProsima_user_DllExport virtual bool getKey(
                void* data,
                eprosima::fastrtps::rtps::InstanceHandle_t* ihandle,
                bool force_md5 = false) override;

        eProsima_user_DllExport virtual void* createData() override;

        eProsima_user_DllExport virtual void deleteData(
                void* data) override;

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
        eProsima_user_DllExport inline bool is_bounded() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED

    #ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN
        eProsima_user_DllExport inline bool is_plain() const override
        {
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_IS_PLAIN

    #ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
        eProsima_user_DllExport inline bool construct_sample(
                void* memory) const override
        {
            new (memory) YanpitStatus();
            return true;
        }

    #endif  // TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE

        MD5 m_md5;
        unsigned char* m_keyBuffer;
    };
}

#endif // _FAST_DDS_GENERATED_YANPIT_YANPIT_PUBSUBTYPES_H_