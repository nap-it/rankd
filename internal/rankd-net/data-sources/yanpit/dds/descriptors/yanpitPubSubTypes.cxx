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
 * @file yanpitPubSubTypes.cpp
 * This header file contains the implementation of the serialization functions.
 *
 * This file was generated by the tool fastcdrgen.
 */


#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>

#include "reporting/dds/descriptors/yanpitPubSubTypes.h"

using SerializedPayload_t = eprosima::fastrtps::rtps::SerializedPayload_t;
using InstanceHandle_t = eprosima::fastrtps::rtps::InstanceHandle_t;

namespace Yanpit {
    IperfStatusPubSubType::IperfStatusPubSubType()
    {
        setName("Yanpit::IperfStatus");
        auto type_size = IperfStatus::getMaxCdrSerializedSize();
        type_size += eprosima::fastcdr::Cdr::alignment(type_size, 4); /* possible submessage alignment */
        m_typeSize = static_cast<uint32_t>(type_size) + 4; /*encapsulation*/
        m_isGetKeyDefined = IperfStatus::isKeyDefined();
        size_t keyLength = IperfStatus::getKeyMaxCdrSerializedSize() > 16 ?
                IperfStatus::getKeyMaxCdrSerializedSize() : 16;
        m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
        memset(m_keyBuffer, 0, keyLength);
    }

    IperfStatusPubSubType::~IperfStatusPubSubType()
    {
        if (m_keyBuffer != nullptr)
        {
            free(m_keyBuffer);
        }
    }

    bool IperfStatusPubSubType::serialize(
            void* data,
            SerializedPayload_t* payload)
    {
        IperfStatus* p_type = static_cast<IperfStatus*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
        payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
        // Serialize encapsulation
        ser.serialize_encapsulation();

        try
        {
            // Serialize the object.
            p_type->serialize(ser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        // Get the serialized length
        payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
        return true;
    }

    bool IperfStatusPubSubType::deserialize(
            SerializedPayload_t* payload,
            void* data)
    {
        try
        {
            //Convert DATA to pointer of your type
            IperfStatus* p_type = static_cast<IperfStatus*>(data);

            // Object that manages the raw buffer.
            eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

            // Object that deserializes the data.
            eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

            // Deserialize encapsulation.
            deser.read_encapsulation();
            payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

            // Deserialize the object.
            p_type->deserialize(deser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        return true;
    }

    std::function<uint32_t()> IperfStatusPubSubType::getSerializedSizeProvider(
            void* data)
    {
        return [data]() -> uint32_t
               {
                   return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<IperfStatus*>(data))) +
                          4u /*encapsulation*/;
               };
    }

    void* IperfStatusPubSubType::createData()
    {
        return reinterpret_cast<void*>(new IperfStatus());
    }

    void IperfStatusPubSubType::deleteData(
            void* data)
    {
        delete(reinterpret_cast<IperfStatus*>(data));
    }

    bool IperfStatusPubSubType::getKey(
            void* data,
            InstanceHandle_t* handle,
            bool force_md5)
    {
        if (!m_isGetKeyDefined)
        {
            return false;
        }

        IperfStatus* p_type = static_cast<IperfStatus*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
                IperfStatus::getKeyMaxCdrSerializedSize());

        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
        p_type->serializeKey(ser);
        if (force_md5 || IperfStatus::getKeyMaxCdrSerializedSize() > 16)
        {
            m_md5.init();
            m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
            m_md5.finalize();
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_md5.digest[i];
            }
        }
        else
        {
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_keyBuffer[i];
            }
        }
        return true;
    }

    ActiveToolsPubSubType::ActiveToolsPubSubType()
    {
        setName("Yanpit::ActiveTools");
        auto type_size = ActiveTools::getMaxCdrSerializedSize();
        type_size += eprosima::fastcdr::Cdr::alignment(type_size, 4); /* possible submessage alignment */
        m_typeSize = static_cast<uint32_t>(type_size) + 4; /*encapsulation*/
        m_isGetKeyDefined = ActiveTools::isKeyDefined();
        size_t keyLength = ActiveTools::getKeyMaxCdrSerializedSize() > 16 ?
                ActiveTools::getKeyMaxCdrSerializedSize() : 16;
        m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
        memset(m_keyBuffer, 0, keyLength);
    }

    ActiveToolsPubSubType::~ActiveToolsPubSubType()
    {
        if (m_keyBuffer != nullptr)
        {
            free(m_keyBuffer);
        }
    }

    bool ActiveToolsPubSubType::serialize(
            void* data,
            SerializedPayload_t* payload)
    {
        ActiveTools* p_type = static_cast<ActiveTools*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
        payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
        // Serialize encapsulation
        ser.serialize_encapsulation();

        try
        {
            // Serialize the object.
            p_type->serialize(ser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        // Get the serialized length
        payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
        return true;
    }

    bool ActiveToolsPubSubType::deserialize(
            SerializedPayload_t* payload,
            void* data)
    {
        try
        {
            //Convert DATA to pointer of your type
            ActiveTools* p_type = static_cast<ActiveTools*>(data);

            // Object that manages the raw buffer.
            eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

            // Object that deserializes the data.
            eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

            // Deserialize encapsulation.
            deser.read_encapsulation();
            payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

            // Deserialize the object.
            p_type->deserialize(deser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        return true;
    }

    std::function<uint32_t()> ActiveToolsPubSubType::getSerializedSizeProvider(
            void* data)
    {
        return [data]() -> uint32_t
               {
                   return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<ActiveTools*>(data))) +
                          4u /*encapsulation*/;
               };
    }

    void* ActiveToolsPubSubType::createData()
    {
        return reinterpret_cast<void*>(new ActiveTools());
    }

    void ActiveToolsPubSubType::deleteData(
            void* data)
    {
        delete(reinterpret_cast<ActiveTools*>(data));
    }

    bool ActiveToolsPubSubType::getKey(
            void* data,
            InstanceHandle_t* handle,
            bool force_md5)
    {
        if (!m_isGetKeyDefined)
        {
            return false;
        }

        ActiveTools* p_type = static_cast<ActiveTools*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
                ActiveTools::getKeyMaxCdrSerializedSize());

        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
        p_type->serializeKey(ser);
        if (force_md5 || ActiveTools::getKeyMaxCdrSerializedSize() > 16)
        {
            m_md5.init();
            m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
            m_md5.finalize();
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_md5.digest[i];
            }
        }
        else
        {
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_keyBuffer[i];
            }
        }
        return true;
    }

    NcmStatusPubSubType::NcmStatusPubSubType()
    {
        setName("Yanpit::NcmStatus");
        auto type_size = NcmStatus::getMaxCdrSerializedSize();
        type_size += eprosima::fastcdr::Cdr::alignment(type_size, 4); /* possible submessage alignment */
        m_typeSize = static_cast<uint32_t>(type_size) + 4; /*encapsulation*/
        m_isGetKeyDefined = NcmStatus::isKeyDefined();
        size_t keyLength = NcmStatus::getKeyMaxCdrSerializedSize() > 16 ?
                NcmStatus::getKeyMaxCdrSerializedSize() : 16;
        m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
        memset(m_keyBuffer, 0, keyLength);
    }

    NcmStatusPubSubType::~NcmStatusPubSubType()
    {
        if (m_keyBuffer != nullptr)
        {
            free(m_keyBuffer);
        }
    }

    bool NcmStatusPubSubType::serialize(
            void* data,
            SerializedPayload_t* payload)
    {
        NcmStatus* p_type = static_cast<NcmStatus*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
        payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
        // Serialize encapsulation
        ser.serialize_encapsulation();

        try
        {
            // Serialize the object.
            p_type->serialize(ser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        // Get the serialized length
        payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
        return true;
    }

    bool NcmStatusPubSubType::deserialize(
            SerializedPayload_t* payload,
            void* data)
    {
        try
        {
            //Convert DATA to pointer of your type
            NcmStatus* p_type = static_cast<NcmStatus*>(data);

            // Object that manages the raw buffer.
            eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

            // Object that deserializes the data.
            eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

            // Deserialize encapsulation.
            deser.read_encapsulation();
            payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

            // Deserialize the object.
            p_type->deserialize(deser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        return true;
    }

    std::function<uint32_t()> NcmStatusPubSubType::getSerializedSizeProvider(
            void* data)
    {
        return [data]() -> uint32_t
               {
                   return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<NcmStatus*>(data))) +
                          4u /*encapsulation*/;
               };
    }

    void* NcmStatusPubSubType::createData()
    {
        return reinterpret_cast<void*>(new NcmStatus());
    }

    void NcmStatusPubSubType::deleteData(
            void* data)
    {
        delete(reinterpret_cast<NcmStatus*>(data));
    }

    bool NcmStatusPubSubType::getKey(
            void* data,
            InstanceHandle_t* handle,
            bool force_md5)
    {
        if (!m_isGetKeyDefined)
        {
            return false;
        }

        NcmStatus* p_type = static_cast<NcmStatus*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
                NcmStatus::getKeyMaxCdrSerializedSize());

        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
        p_type->serializeKey(ser);
        if (force_md5 || NcmStatus::getKeyMaxCdrSerializedSize() > 16)
        {
            m_md5.init();
            m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
            m_md5.finalize();
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_md5.digest[i];
            }
        }
        else
        {
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_keyBuffer[i];
            }
        }
        return true;
    }

    LocalnetStatusPubSubType::LocalnetStatusPubSubType()
    {
        setName("Yanpit::LocalnetStatus");
        auto type_size = LocalnetStatus::getMaxCdrSerializedSize();
        type_size += eprosima::fastcdr::Cdr::alignment(type_size, 4); /* possible submessage alignment */
        m_typeSize = static_cast<uint32_t>(type_size) + 4; /*encapsulation*/
        m_isGetKeyDefined = LocalnetStatus::isKeyDefined();
        size_t keyLength = LocalnetStatus::getKeyMaxCdrSerializedSize() > 16 ?
                LocalnetStatus::getKeyMaxCdrSerializedSize() : 16;
        m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
        memset(m_keyBuffer, 0, keyLength);
    }

    LocalnetStatusPubSubType::~LocalnetStatusPubSubType()
    {
        if (m_keyBuffer != nullptr)
        {
            free(m_keyBuffer);
        }
    }

    bool LocalnetStatusPubSubType::serialize(
            void* data,
            SerializedPayload_t* payload)
    {
        LocalnetStatus* p_type = static_cast<LocalnetStatus*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
        payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
        // Serialize encapsulation
        ser.serialize_encapsulation();

        try
        {
            // Serialize the object.
            p_type->serialize(ser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        // Get the serialized length
        payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
        return true;
    }

    bool LocalnetStatusPubSubType::deserialize(
            SerializedPayload_t* payload,
            void* data)
    {
        try
        {
            //Convert DATA to pointer of your type
            LocalnetStatus* p_type = static_cast<LocalnetStatus*>(data);

            // Object that manages the raw buffer.
            eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

            // Object that deserializes the data.
            eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

            // Deserialize encapsulation.
            deser.read_encapsulation();
            payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

            // Deserialize the object.
            p_type->deserialize(deser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        return true;
    }

    std::function<uint32_t()> LocalnetStatusPubSubType::getSerializedSizeProvider(
            void* data)
    {
        return [data]() -> uint32_t
               {
                   return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<LocalnetStatus*>(data))) +
                          4u /*encapsulation*/;
               };
    }

    void* LocalnetStatusPubSubType::createData()
    {
        return reinterpret_cast<void*>(new LocalnetStatus());
    }

    void LocalnetStatusPubSubType::deleteData(
            void* data)
    {
        delete(reinterpret_cast<LocalnetStatus*>(data));
    }

    bool LocalnetStatusPubSubType::getKey(
            void* data,
            InstanceHandle_t* handle,
            bool force_md5)
    {
        if (!m_isGetKeyDefined)
        {
            return false;
        }

        LocalnetStatus* p_type = static_cast<LocalnetStatus*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
                LocalnetStatus::getKeyMaxCdrSerializedSize());

        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
        p_type->serializeKey(ser);
        if (force_md5 || LocalnetStatus::getKeyMaxCdrSerializedSize() > 16)
        {
            m_md5.init();
            m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
            m_md5.finalize();
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_md5.digest[i];
            }
        }
        else
        {
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_keyBuffer[i];
            }
        }
        return true;
    }

    PassiveToolsPubSubType::PassiveToolsPubSubType()
    {
        setName("Yanpit::PassiveTools");
        auto type_size = PassiveTools::getMaxCdrSerializedSize();
        type_size += eprosima::fastcdr::Cdr::alignment(type_size, 4); /* possible submessage alignment */
        m_typeSize = static_cast<uint32_t>(type_size) + 4; /*encapsulation*/
        m_isGetKeyDefined = PassiveTools::isKeyDefined();
        size_t keyLength = PassiveTools::getKeyMaxCdrSerializedSize() > 16 ?
                PassiveTools::getKeyMaxCdrSerializedSize() : 16;
        m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
        memset(m_keyBuffer, 0, keyLength);
    }

    PassiveToolsPubSubType::~PassiveToolsPubSubType()
    {
        if (m_keyBuffer != nullptr)
        {
            free(m_keyBuffer);
        }
    }

    bool PassiveToolsPubSubType::serialize(
            void* data,
            SerializedPayload_t* payload)
    {
        PassiveTools* p_type = static_cast<PassiveTools*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
        payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
        // Serialize encapsulation
        ser.serialize_encapsulation();

        try
        {
            // Serialize the object.
            p_type->serialize(ser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        // Get the serialized length
        payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
        return true;
    }

    bool PassiveToolsPubSubType::deserialize(
            SerializedPayload_t* payload,
            void* data)
    {
        try
        {
            //Convert DATA to pointer of your type
            PassiveTools* p_type = static_cast<PassiveTools*>(data);

            // Object that manages the raw buffer.
            eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

            // Object that deserializes the data.
            eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

            // Deserialize encapsulation.
            deser.read_encapsulation();
            payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

            // Deserialize the object.
            p_type->deserialize(deser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        return true;
    }

    std::function<uint32_t()> PassiveToolsPubSubType::getSerializedSizeProvider(
            void* data)
    {
        return [data]() -> uint32_t
               {
                   return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<PassiveTools*>(data))) +
                          4u /*encapsulation*/;
               };
    }

    void* PassiveToolsPubSubType::createData()
    {
        return reinterpret_cast<void*>(new PassiveTools());
    }

    void PassiveToolsPubSubType::deleteData(
            void* data)
    {
        delete(reinterpret_cast<PassiveTools*>(data));
    }

    bool PassiveToolsPubSubType::getKey(
            void* data,
            InstanceHandle_t* handle,
            bool force_md5)
    {
        if (!m_isGetKeyDefined)
        {
            return false;
        }

        PassiveTools* p_type = static_cast<PassiveTools*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
                PassiveTools::getKeyMaxCdrSerializedSize());

        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
        p_type->serializeKey(ser);
        if (force_md5 || PassiveTools::getKeyMaxCdrSerializedSize() > 16)
        {
            m_md5.init();
            m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
            m_md5.finalize();
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_md5.digest[i];
            }
        }
        else
        {
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_keyBuffer[i];
            }
        }
        return true;
    }

    YanpitStatusPubSubType::YanpitStatusPubSubType()
    {
        setName("Yanpit::YanpitStatus");
        auto type_size = YanpitStatus::getMaxCdrSerializedSize();
        type_size += eprosima::fastcdr::Cdr::alignment(type_size, 4); /* possible submessage alignment */
        m_typeSize = static_cast<uint32_t>(type_size) + 4; /*encapsulation*/
        m_isGetKeyDefined = YanpitStatus::isKeyDefined();
        size_t keyLength = YanpitStatus::getKeyMaxCdrSerializedSize() > 16 ?
                YanpitStatus::getKeyMaxCdrSerializedSize() : 16;
        m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
        memset(m_keyBuffer, 0, keyLength);
    }

    YanpitStatusPubSubType::~YanpitStatusPubSubType()
    {
        if (m_keyBuffer != nullptr)
        {
            free(m_keyBuffer);
        }
    }

    bool YanpitStatusPubSubType::serialize(
            void* data,
            SerializedPayload_t* payload)
    {
        YanpitStatus* p_type = static_cast<YanpitStatus*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
        payload->encapsulation = ser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
        // Serialize encapsulation
        ser.serialize_encapsulation();

        try
        {
            // Serialize the object.
            p_type->serialize(ser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        // Get the serialized length
        payload->length = static_cast<uint32_t>(ser.getSerializedDataLength());
        return true;
    }

    bool YanpitStatusPubSubType::deserialize(
            SerializedPayload_t* payload,
            void* data)
    {
        try
        {
            //Convert DATA to pointer of your type
            YanpitStatus* p_type = static_cast<YanpitStatus*>(data);

            // Object that manages the raw buffer.
            eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(payload->data), payload->length);

            // Object that deserializes the data.
            eprosima::fastcdr::Cdr deser(fastbuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);

            // Deserialize encapsulation.
            deser.read_encapsulation();
            payload->encapsulation = deser.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

            // Deserialize the object.
            p_type->deserialize(deser);
        }
        catch (eprosima::fastcdr::exception::NotEnoughMemoryException& /*exception*/)
        {
            return false;
        }

        return true;
    }

    std::function<uint32_t()> YanpitStatusPubSubType::getSerializedSizeProvider(
            void* data)
    {
        return [data]() -> uint32_t
               {
                   return static_cast<uint32_t>(type::getCdrSerializedSize(*static_cast<YanpitStatus*>(data))) +
                          4u /*encapsulation*/;
               };
    }

    void* YanpitStatusPubSubType::createData()
    {
        return reinterpret_cast<void*>(new YanpitStatus());
    }

    void YanpitStatusPubSubType::deleteData(
            void* data)
    {
        delete(reinterpret_cast<YanpitStatus*>(data));
    }

    bool YanpitStatusPubSubType::getKey(
            void* data,
            InstanceHandle_t* handle,
            bool force_md5)
    {
        if (!m_isGetKeyDefined)
        {
            return false;
        }

        YanpitStatus* p_type = static_cast<YanpitStatus*>(data);

        // Object that manages the raw buffer.
        eprosima::fastcdr::FastBuffer fastbuffer(reinterpret_cast<char*>(m_keyBuffer),
                YanpitStatus::getKeyMaxCdrSerializedSize());

        // Object that serializes the data.
        eprosima::fastcdr::Cdr ser(fastbuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);
        p_type->serializeKey(ser);
        if (force_md5 || YanpitStatus::getKeyMaxCdrSerializedSize() > 16)
        {
            m_md5.init();
            m_md5.update(m_keyBuffer, static_cast<unsigned int>(ser.getSerializedDataLength()));
            m_md5.finalize();
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_md5.digest[i];
            }
        }
        else
        {
            for (uint8_t i = 0; i < 16; ++i)
            {
                handle->value[i] = m_keyBuffer[i];
            }
        }
        return true;
    }


} //End of namespace Yanpit