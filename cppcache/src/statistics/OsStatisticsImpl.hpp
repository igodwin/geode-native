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

#ifndef GEODE_STATISTICS_OSSTATISTICSIMPL_H_
#define GEODE_STATISTICS_OSSTATISTICSIMPL_H_

#include "Statistics.hpp"
#include "StatisticsFactory.hpp"
#include "StatisticsTypeImpl.hpp"
#include "../NonCopyable.hpp"

/** @file
 */

namespace apache {
namespace geode {
namespace statistics {

using namespace apache::geode::client;

/**
 * An implementation of {@link Statistics} that stores its statistics
 * in local memory and does not support atomic operations.
 *
 */
class OsStatisticsImpl : public Statistics,
                         private NonCopyable,
                         private NonAssignable {
 private:
  /** The type of this statistics instance */
  StatisticsTypeImpl* statsType;

  /** The display name of this statistics instance */
  const std::string textId;

  /** Numeric information display with these statistics */
  int64_t numericId;

  /** Are these statistics closed? */
  bool closed;

  /** Uniquely identifies this instance */
  int64_t uniqueId;

  /****************************************************************************/
  /** An array containing the values of the int32_t statistics */
  int32_t* intStorage;

  /** An array containing the values of the int64_t  statistics */
  int64_t* longStorage;

  /** An array containing the values of the double statistics */
  double* doubleStorage;

  ///////////////////////Private Methods//////////////////////////
  bool isOpen() const;

  int32_t getIntId(const StatisticDescriptor* descriptor) const;

  int32_t getLongId(const StatisticDescriptor* descriptor) const;

  int32_t getDoubleId(const StatisticDescriptor* descriptor) const;

  //////////////////////  Static private Methods  //////////////////////

  static int64_t calcNumericId(StatisticsFactory* system, int64_t userValue);

  static std::string calcTextId(StatisticsFactory* system,
                                const std::string& userValue);

  ///////////////////////  Constructors  ///////////////////////

  /**
   * Creates a new statistics instance of the given type
   *
   * @param type
   *        A description of the statistics
   * @param textId
   *        Text that identifies this statistic when it is monitored
   * @param numericId
   *        A number that displayed when this statistic is monitored
   * @param uniqueId
   *        A number that uniquely identifies this instance
   * @param system
   *        The distributed system that determines whether or not these
   *        statistics are stored (and collected) in local memory
   */

 public:
  OsStatisticsImpl(StatisticsType* type, const std::string& textId,
                   int64_t numericId, int64_t uniqueId,
                   StatisticsFactory* system);

  ~OsStatisticsImpl();

  //////////////////////  Instance Methods  //////////////////////

  int32_t nameToId(const std::string& name) const override;

  StatisticDescriptor* nameToDescriptor(const std::string& name) const override;

  bool isClosed() const override;

  bool isShared() const override;

  bool isAtomic() const override;

  void close() override;
  /////////////////////////Attribute methods//////////////////////////

  StatisticsType* getType() const override;

  const std::string& getTextId() const override;

  int64_t getNumericId() const override;

  int64_t getUniqueId() const override;

  ////////////////////////  set() Methods  ///////////////////////

  void setInt(const std::string& name, int32_t value) override;

  void setInt(const StatisticDescriptor* descriptor, int32_t value) override;

  void setInt(int32_t id, int32_t value) override;

  void setLong(const std::string& name, int64_t value) override;

  void setLong(const StatisticDescriptor* descriptor, int64_t value) override;

  void setLong(int32_t id, int64_t value) override;

  void setDouble(const std::string& name, double value) override;

  void setDouble(const StatisticDescriptor* descriptor, double value) override;

  void setDouble(int32_t id, double value) override;

  ///////////////////////  get() Methods  ///////////////////////

  int32_t getInt(const std::string& name) const override;

  int32_t getInt(const StatisticDescriptor* descriptor) const override;

  int32_t getInt(int32_t id) const override;

  int64_t getLong(const std::string& name) const override;

  int64_t getLong(const StatisticDescriptor* descriptor) const override;

  int64_t getLong(int32_t id) const override;

  double getDouble(const std::string& name) const override;

  double getDouble(const StatisticDescriptor* descriptor) const override;

  double getDouble(int32_t id) const override;

  int64_t getRawBits(const StatisticDescriptor* descriptor) const override;

  ////////////////////////  inc() Methods  ////////////////////////

  int32_t incInt(const std::string& name, int32_t delta) override;

  int32_t incInt(const StatisticDescriptor* descriptor, int32_t delta) override;

  int32_t incInt(int32_t id, int32_t delta) override;

  int64_t incLong(const std::string& name, int64_t delta) override;

  int64_t incLong(const StatisticDescriptor* descriptor,
                  int64_t delta) override;

  int64_t incLong(int32_t id, int64_t delta) override;

  double incDouble(const std::string& name, double delta) override;

  double incDouble(const StatisticDescriptor* descriptor,
                   double delta) override;

  double incDouble(int32_t id, double delta) override;

  ////////////////////////  store() Methods  ///////////////////////
 protected:
  /**
   * Sets the value of a statistic of type <code>int</code> at the
   * given offset, but performs no type checking.
   */
  void _setInt(int32_t offset, int32_t value);

  void _setLong(int32_t offset, int64_t value);

  void _setDouble(int32_t offset, double value);
  ///////////////////////  get() Methods  ///////////////////////
  /**
   * Returns the value of the statistic of type <code>int</code> at
   * the given offset, but performs no type checking.
   */
  int32_t _getInt(int32_t offset) const;

  int64_t _getLong(int32_t offset) const;

  double _getDouble(int32_t offset) const;

  /**
   * Returns the bits that represent the raw value of the
   * specified statistic descriptor.
   */
  int64_t _getRawBits(const StatisticDescriptor* stat) const;

  ////////////////////////  inc() Methods  ////////////////////////
  /**
   * Increments the value of the statistic of type <code>int</code> at
   * the given offset by a given amount, but performs no type checking.
   *
   * @return The value of the statistic after it has been incremented
   */
  int32_t _incInt(int32_t offset, int32_t delta);

  int64_t _incLong(int32_t offset, int64_t delta);

  double _incDouble(int32_t offset, double delta);

  /////////////////// internal package methods //////////////////

};  // class

}  // namespace statistics
}  // namespace geode
}  // namespace apache

#endif  // GEODE_STATISTICS_OSSTATISTICSIMPL_H_
