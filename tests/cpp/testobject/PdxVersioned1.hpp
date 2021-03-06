#pragma once

#ifndef GEODE_TESTOBJECT_PDXVERSIONED1_H_
#define GEODE_TESTOBJECT_PDXVERSIONED1_H_

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
/*
 * PdxObject.hpp
 *
 *  Created on: Sep 29, 2011
 *      Author: npatel
 */

#include <geode/PdxSerializable.hpp>
#include <geode/CacheableEnum.hpp>
#include <geode/PdxWriter.hpp>
#include <geode/PdxReader.hpp>
#include "fwklib/FwkExport.hpp"

#ifdef _WIN32
#ifdef BUILD_TESTOBJECT
#define TESTOBJECT_EXPORT LIBEXP
#else
#define TESTOBJECT_EXPORT LIBIMP
#endif
#else
#define TESTOBJECT_EXPORT
#endif

using namespace apache::geode::client;

namespace PdxTests {

class TESTOBJECT_EXPORT PdxVersioned1 : public PdxSerializable {
 private:
  char16_t m_char;
  bool m_bool;
  int8_t m_byte;
  int8_t m_sbyte;  //
  int16_t m_int16;
  int16_t m_uint16;  ///
  int32_t m_int32;
  // int32_t m_uint32;///
  // int64_t m_long;
  int64_t m_ulong;  ///
  float m_float;
  double m_double;

  char* m_string;

  bool* m_boolArray;
  int8_t* m_byteArray;
  int8_t* m_sbyteArray;  ///

  wchar_t* m_charArray;

  std::shared_ptr<CacheableDate> m_date;

  int16_t* m_int16Array;
  int16_t* m_uint16Array;

  int32_t* m_int32Array;
  int32_t* m_uint32Array;

  int64_t* m_longArray;
  int64_t* m_ulongArray;

  float* m_floatArray;
  double* m_doubleArray;

  int8_t** m_byteByteArray;

  char** m_stringArray;
  std::shared_ptr<CacheableArrayList> m_arraylist;
  std::shared_ptr<CacheableHashMap> m_map;

  int8_t* m_byte252;
  int8_t* m_byte253;
  int8_t* m_byte65535;
  int8_t* m_byte65536;
  enum pdxEnumTest { pdx1, pdx2, pdx3, pdx4 };
  std::shared_ptr<Cacheable> m_pdxEnum;

  int32_t boolArrayLen;
  int32_t charArrayLen;
  int32_t byteArrayLen;
  int32_t shortArrayLen;
  int32_t intArrayLen;
  int32_t longArrayLen;
  int32_t doubleArrayLen;
  int32_t floatArrayLen;
  int32_t strLenArray;
  int32_t m_byte252Len;
  int32_t m_byte253Len;
  int32_t m_byte65535Len;
  int32_t m_byte65536Len;
  int32_t byteByteArrayLen;

  int* lengthArr;

 public:
  /*  PdxVersioned1(const char* key) {
            LOGINFO("rjk:inside testobject pdxType");
      init(key);
    }*/
  PdxVersioned1() { init("def"); }
  PdxVersioned1(const char* key);
  void init(const char* key);
  inline bool compareBool(bool b, bool b2) {
    if (b == b2) return b;
    throw IllegalStateException("Not got expected value for bool type: ");
  }

  virtual ~PdxVersioned1() {}

  virtual size_t objectSize() const override {
    auto objectSize = sizeof(PdxVersioned1);
    return objectSize;
  }
  // void checkNullAndDelete(void *data);
  wchar_t getChar() { return m_char; }

  wchar_t* getCharArray() { return m_charArray; }

  int8_t** getArrayOfByteArrays() { return m_byteByteArray; }

  bool getBool() { return m_bool; }

  std::shared_ptr<CacheableHashMap> getHashMap() { return m_map; }

  int8_t getSByte() { return m_sbyte; }

  int16_t getUint16() { return m_uint16; }

  // int32_t getUInt() {
  //   return m_uint32;
  //}

  int64_t getULong() { return m_ulong; }

  int16_t* getUInt16Array() { return m_uint16Array; }

  int32_t* getUIntArray() { return m_uint32Array; }

  int64_t* getULongArray() { return m_ulongArray; }

  int8_t* getByte252() { return m_byte252; }

  int8_t* getByte253() { return m_byte253; }

  int8_t* getByte65535() { return m_byte65535; }

  int8_t* getByte65536() { return m_byte65536; }

  int8_t* getSByteArray() { return m_sbyteArray; }

  std::shared_ptr<CacheableArrayList> getArrayList() { return m_arraylist; }

  int8_t getByte() { return m_byte; }

  int16_t getShort() { return m_int16; }

  int32_t getInt() { return m_int32; }

  // int64_t getLong(){
  //   return m_long;
  // }

  float getFloat() { return m_float; }

  double getDouble() { return m_double; }

  const char* getString() { return m_string; }

  bool* getBoolArray() { return m_boolArray; }

  int8_t* getByteArray() { return m_byteArray; }

  int16_t* getShortArray() { return m_int16Array; }

  int32_t* getIntArray() { return m_int32Array; }

  int64_t* getLongArray() { return m_longArray; }

  double* getDoubleArray() { return m_doubleArray; }

  float* getFloatArray() { return m_floatArray; }

  char** getStringArray() { return m_stringArray; }

  std::shared_ptr<CacheableDate> getDate() { return m_date; }

  std::shared_ptr<CacheableEnum> getEnum() {
    return std::static_pointer_cast<CacheableEnum>(m_pdxEnum);
  }

  int32_t getByteArrayLength() { return byteArrayLen; }

  int32_t getBoolArrayLength() { return boolArrayLen; }

  int32_t getShortArrayLength() { return shortArrayLen; }

  int32_t getStringArrayLength() { return strLenArray; }

  int32_t getIntArrayLength() { return intArrayLen; }

  int32_t getLongArrayLength() { return longArrayLen; }

  int32_t getFloatArrayLength() { return floatArrayLen; }

  int32_t getDoubleArrayLength() { return doubleArrayLen; }

  int32_t getbyteByteArrayLength() { return byteByteArrayLen; }

  int32_t getCharArrayLength() { return charArrayLen; }

  using PdxSerializable::toData;
  using PdxSerializable::fromData;

  virtual void toData(std::shared_ptr<PdxWriter> pw) const override;

  virtual void fromData(std::shared_ptr<PdxReader> pr) override;

  std::string toString() const override;

  const std::string& getClassName() const override {
    static std::string className = "PdxTests::PdxVersioned";
    return className;
  }

  static PdxSerializable* createDeserializable() { return new PdxVersioned1(); }

  bool equals(PdxTests::PdxVersioned1& other, bool isPdxReadSerialized) const;

  template <typename T1, typename T2>
  bool genericValCompare(T1 value1, T2 value2) const;

  template <typename T1, typename T2>
  bool genericCompare(T1* value1, T2* value2, int length) const;

  template <typename T1, typename T2>
  bool generic2DCompare(T1** value1, T2** value2, int length,
                        int* arrLengths) const;
};

}  // namespace PdxTests

#endif  // GEODE_TESTOBJECT_PDXVERSIONED1_H_
