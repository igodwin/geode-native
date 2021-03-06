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

#ifndef GEODE_PDXFIELDTYPES_H_
#define GEODE_PDXFIELDTYPES_H_

#include "geode_base.hpp"

namespace apache {
namespace geode {
namespace client {

class CPPCACHE_EXPORT PdxFieldTypes {
 public:
  enum PdxFieldType {
    BOOLEAN,
    BYTE,
    CHAR,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    DATE,
    STRING,
    OBJECT,
    BOOLEAN_ARRAY,
    CHAR_ARRAY,
    BYTE_ARRAY,
    SHORT_ARRAY,
    INT_ARRAY,
    LONG_ARRAY,
    FLOAT_ARRAY,
    DOUBLE_ARRAY,
    STRING_ARRAY,
    OBJECT_ARRAY,
    ARRAY_OF_BYTE_ARRAYS
  };
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_PDXFIELDTYPES_H_
