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

#pragma once

#ifndef GEODE_ENUMINFO_H_
#define GEODE_ENUMINFO_H_

#include <geode/GeodeTypeIds.hpp>
#include <geode/CacheableString.hpp>
#include <geode/CacheableKey.hpp>

namespace apache {
namespace geode {
namespace client {

class CPPCACHE_EXPORT EnumInfo : public CacheableKey {
 private:
  std::shared_ptr<CacheableString> m_enumClassName;
  std::shared_ptr<CacheableString> m_enumName;
  int32_t m_ordinal;

 public:
  ~EnumInfo();
  EnumInfo();
  EnumInfo(const char* enumClassName, const char* enumName, int32_t m_ordinal);
  static Serializable* createDeserializable() { return new EnumInfo(); }
  virtual void toData(DataOutput& output) const override;
  virtual void fromData(DataInput& input) override;
  virtual size_t objectSize() const override {
    auto size = sizeof(EnumInfo);
    size += sizeof(int32_t);
    size += m_enumClassName->objectSize();
    size += m_enumName->objectSize();
    return size;
  }
  virtual int32_t classId() const override { return 0; }
  virtual int8_t typeId() const override { return GeodeTypeIds::EnumInfo; }
  virtual std::string toString() const override { return "EnumInfo"; }
  virtual bool operator==(const CacheableKey& other) const override;
  virtual int32_t hashcode() const override;

  virtual int8_t DSFID() const override;
  std::shared_ptr<CacheableString> getEnumClassName() const {
    return m_enumClassName;
  }
  std::shared_ptr<CacheableString> getEnumName() const { return m_enumName; }
  int32_t getEnumOrdinal() const { return m_ordinal; }
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_ENUMINFO_H_
