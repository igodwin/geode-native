#pragma once

#ifndef GEODE_PROXYREMOTEQUERYSERVICE_H_
#define GEODE_PROXYREMOTEQUERYSERVICE_H_

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
#include <geode/geode_globals.hpp>
#include <memory>
#include "CqService.hpp"
#include "UserAttributes.hpp"
#include <geode/QueryService.hpp>
#include "ProxyCache.hpp"
#include "ThinClientCacheDistributionManager.hpp"

#include <ace/Recursive_Thread_Mutex.h>

namespace apache {
namespace geode {
namespace client {

class CacheImpl;
class ThinClientPoolDM;

class CPPCACHE_EXPORT ProxyRemoteQueryService : public QueryService {
 public:
  ProxyRemoteQueryService(std::shared_ptr<ProxyCache> cptr);
  virtual ~ProxyRemoteQueryService() = default;

  std::shared_ptr<Query> newQuery(std::string querystring) override;

  virtual std::shared_ptr<CqQuery> newCq(
      std::string querystr, const std::shared_ptr<CqAttributes>& cqAttr,
      bool isDurable = false) override;

  virtual std::shared_ptr<CqQuery> newCq(
      std::string name, std::string querystr,
      const std::shared_ptr<CqAttributes>& cqAttr,
      bool isDurable = false) override;

  virtual void closeCqs() override;

  virtual QueryService::query_container_type getCqs() const override;

  virtual std::shared_ptr<CqQuery> getCq(
      const std::string& name) const override;

  virtual void executeCqs() override;

  virtual void stopCqs() override;

  virtual std::shared_ptr<CqServiceStatistics> getCqServiceStatistics()
      const override;

  virtual std::shared_ptr<CacheableArrayList> getAllDurableCqsFromServer()
      const override;

 private:
  static void unSupportedException(const std::string& operationName);
  void addCqQuery(const std::shared_ptr<CqQuery>& cqQuery);
  void closeCqs(bool keepAlive);

  std::shared_ptr<QueryService> m_realQueryService;
  std::shared_ptr<ProxyCache> m_proxyCache;
  query_container_type m_cqQueries;
  // lock for cqQuery list;
  mutable ACE_Recursive_Thread_Mutex m_cqQueryListLock;

  friend class ProxyCache;
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_PROXYREMOTEQUERYSERVICE_H_
