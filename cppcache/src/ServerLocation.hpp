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

#ifndef GEODE_SERVERLOCATION_H_
#define GEODE_SERVERLOCATION_H_

#include <geode/Serializable.hpp>
#include <string>
#include <geode/DataInput.hpp>
#include "Utils.hpp"
#include <geode/DataOutput.hpp>
#include <geode/CacheableString.hpp>
#include "GeodeTypeIdsImpl.hpp"
#include <ace/INET_Addr.h>
#include <geode/CacheableBuiltins.hpp>

namespace apache {
namespace geode {
namespace client {

class CPPCACHE_EXPORT ServerLocation : public Serializable {
 public:
  ServerLocation(std::string serverName, int port)
      : Serializable(), m_port(port) {
    if (serverName.length() > 0) {
      m_serverName = CacheableString::create(serverName.c_str());
      LOGDEBUG(
          "ServerLocation::ServerLocation(): creating ServerLocation for %s:%d",
          serverName.c_str(), port);
    }
    makeEpString();
  }
  ServerLocation()
      : Serializable(),
        m_serverName(nullptr),
        m_port(-1)  // Default constructor for deserialiozation.
  {}

  ServerLocation(std::string name) {
    /*
    name = Utils::convertHostToCanonicalForm(name.c_str());
    */
    int32_t position = static_cast<int32_t>(name.find_first_of(":"));
    std::string serverName;
    serverName = name.substr(0, position);
    m_port = atoi((name.substr(position + 1)).c_str());
    m_serverName = CacheableString::create(serverName.c_str());
    makeEpString();
  }

  std::string getServerName() const {
    if (m_serverName != nullptr) {
      return m_serverName->value();
    }
    return "";
  }
  void setServername(std::shared_ptr<CacheableString> sn) { m_serverName = sn; }
  int getPort() const { return m_port; }
  void toData(DataOutput& output) const {
    if (m_serverName != nullptr) {
      output.writeNativeString(m_serverName->toString().c_str());
    }
    output.writeInt(m_port);
  }
  void fromData(DataInput& input) {
    m_serverName = input.readNativeString();
    m_port = input.readInt32();
    makeEpString();
  }
  size_t objectSize() const {
    size_t size = sizeof(ServerLocation);
    if (m_serverName) {
      size += m_serverName->objectSize();
    }
    return size;
  }
  int8_t typeId() const {
    return 0;  // NOt needed infact
  }
  int8_t DSFID() const {
    return static_cast<int8_t>(GeodeTypeIdsImpl::FixedIDByte);  // Never used
  }

  int32_t classId() const {
    return 0;  // Never used
  }
  ServerLocation& operator=(const ServerLocation& rhs) {
    if (this == &rhs) return *this;
    this->m_serverName = rhs.m_serverName;
    this->m_port = rhs.m_port;
    // makeEpString();
    return *this;
  }
  ServerLocation(const ServerLocation& rhs) {
    this->m_serverName = rhs.m_serverName;
    this->m_port = rhs.m_port;
    // makeEpString();
  }
  void printInfo() {
    LOGDEBUG(" Got Host %s, and port %d", getServerName().c_str(), m_port);
  }

  bool operator<(const ServerLocation rhs) const {
    auto&& s1 = m_serverName->value();
    auto&& s2 = rhs.m_serverName->value();
    if (s1 < s2) {
      return true;
    } else if (s1 == s2) {
      return (m_port < rhs.m_port);
    } else {
      return false;
    }
  }

  bool operator==(const ServerLocation& rhs) const {
    return (m_serverName->value() == rhs.m_serverName->value()) &&
           (m_port == rhs.m_port);
  }

  inline bool isValid() const {
    if (m_serverName == nullptr) return false;
    return m_serverName->length() > 0 && m_port >= 0;
  }

  inline std::string& getEpString() {
    /*if (m_epString.empty() == false) {
      return m_epString;
    }*/
    return m_epString;
  }

  inline int hashcode() const {
    int prime = 31;
    int result = 1;
    // result = prime * result + ((hostName == null) ? 0 : hostName.hashCode());
    result = prime * result + m_port;
    return result;
  }

  void makeEpString();

 protected:
  std::shared_ptr<CacheableString> m_serverName;
  int m_port;
  std::string m_epString;
};

}  // namespace client
}  // namespace geode
}  // namespace apache

namespace std {

template <>
struct hash<apache::geode::client::ServerLocation> {
  typedef apache::geode::client::ServerLocation argument_type;
  typedef size_t result_type;
  size_t operator()(const apache::geode::client::ServerLocation& val) const {
    return val.hashcode();
  }
};

}  // namespace std

#endif  // GEODE_SERVERLOCATION_H_
