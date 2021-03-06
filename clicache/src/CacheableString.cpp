/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */




#include "DataOutput.hpp"
#include "DataInput.hpp"


#include "CacheableString.hpp"
#include "ExceptionTypes.hpp"

using namespace System;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {

      void CacheableString::ToData(DataOutput^ output)
      {
        if (m_type == GeodeClassIds::CacheableASCIIString ||
            m_type == GeodeClassIds::CacheableString)
        {
          output->WriteUTF(m_value);
        }
        else if (m_type == GeodeClassIds::CacheableASCIIStringHuge)
        {
          output->WriteASCIIHuge(m_value);
        }
        else
        {
          output->WriteUTFHuge(m_value);
        }
      }

      void CacheableString::FromData(DataInput^ input)
      {
        if (m_type == GeodeClassIds::CacheableASCIIString ||
            m_type == GeodeClassIds::CacheableString)
        {
          m_value = input->ReadUTF();
        }
        else if (m_type == GeodeClassIds::CacheableASCIIStringHuge)
        {
          m_value = input->ReadASCIIHuge();
        }
        else
        {
          m_value = input->ReadUTFHuge();
        }
      }


      inline void CacheableString::GetCacheableString(String^ value,
                                                      std::shared_ptr<apache::geode::client::CacheableString>& cStr)
      {
        if (value) {
          cStr = apache::geode::client::CacheableString::create(marshal_as<std::string>(value));
        }
        else {
          cStr.reset(
            static_cast<apache::geode::client::CacheableString*>(apache::geode::client::CacheableString::createDeserializable()));
        }
      }

      inline void CacheableString::GetCacheableString(array<Char>^ value,
                                                      std::shared_ptr<apache::geode::client::CacheableString>& cStr)
      {
        size_t len;
        if (value != nullptr && (len = value->Length) > 0) {
          pin_ptr<const Char> pin_value = &value[0];
          cStr = apache::geode::client::CacheableString::create(
            (const wchar_t*)pin_value, (System::Int32)len);
        }
        else {
          cStr.reset(
            static_cast<apache::geode::client::CacheableString*>(apache::geode::client::CacheableString::createDeserializable()));
        }
      }

      CacheableString::CacheableString(String^ value)
        : CacheableKey()
      {
        if (value == nullptr) {
          throw gcnew IllegalArgumentException("CacheableString: null or " +
                                               "zero-length string provided to the constructor.");
        }
        m_value = value;

        this->SetStringType();
      }

      CacheableString::CacheableString(array<Char>^ value)
        : CacheableKey()
      {
        if (value == nullptr) {
          throw gcnew IllegalArgumentException("CacheableString: null or " +
                                               "zero-length character array provided to the constructor.");
        }
        m_value = gcnew String(value);

        this->SetStringType();
      }

      CacheableString::CacheableString(String^ value, bool noParamCheck)
        : CacheableKey()
      {
        m_value = value;
        this->SetStringType();
      }

      CacheableString::CacheableString(array<Char>^ value, bool noParamCheck)
        : CacheableKey()
      {
        m_value = gcnew String(value);
        this->SetStringType();
      }

      System::UInt64 CacheableString::ObjectSize::get()
      {
        return (m_value->Length * sizeof(char));
      }

      bool CacheableString::Equals(Apache::Geode::Client::ICacheableKey^ other)
      {
        if (other == nullptr || other->ClassId != ClassId) {
          return false;
        }

        CacheableString^ otherStr =
          dynamic_cast<CacheableString^>(other);

        if (otherStr == nullptr)
          return false;

        return m_value->Equals(otherStr->Value);//TODO::
      }

      bool CacheableString::Equals(Object^ obj)
      {
        CacheableString^ otherStr =
          dynamic_cast<CacheableString^>(obj);

        if (otherStr != nullptr) {
          return m_value->Equals(otherStr->Value);
        }
        return false;
      }

      System::Int32 CacheableString::GetHashCode()
      {
        if (String::IsNullOrEmpty(m_value)) {
          return 0;
        }
        //TODO: need to need java hashcode
        //return m_value->GetHashCode();
        if (m_hashcode == 0)
        {
          System::Int32 prime = 31;
          System::Int32 localHash = 0;
          for (System::Int32 i = 0; i < m_value->Length; i++)
            localHash = prime*localHash + m_value[i];
          m_hashcode = localHash;
        }
        return m_hashcode;
      }

      void CacheableString::SetStringType()
      {
        int len = DataOutput::getEncodedLength(m_value);

        if (len == m_value->Length)//ASCII string
        {
          if (len > 0xFFFF)
            m_type = GeodeClassIds::CacheableASCIIStringHuge;
          else
            m_type = GeodeClassIds::CacheableASCIIString;
        }
        else
        {
          if (len > 0xFFFF)
            m_type = GeodeClassIds::CacheableStringHuge;
          else
            m_type = GeodeClassIds::CacheableString;
        }  // namespace Client
      }  // namespace Geode
    }  // namespace Apache

  }
} //namespace 

