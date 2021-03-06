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
#include "fw_dunit.hpp"
#include <geode/FunctionService.hpp>
#include <geode/Execution.hpp>
#include <geode/CqAttributesFactory.hpp>

#define ROOT_NAME "testThinClientSSLWithSecurityAuthz"
#define ROOT_SCOPE DISTRIBUTED_ACK

#include "CacheHelper.hpp"
#include "ThinClientHelper.hpp"
#include "ace/Process.h"

#include "ThinClientSecurity.hpp"

using namespace apache::geode::client::testframework::security;
using namespace apache::geode::client;

const char* locHostPort =
    CacheHelper::getLocatorHostPort(isLocator, isLocalServer, 1);
std::shared_ptr<CredentialGenerator> credentialGeneratorHandler;

std::string getXmlPath() {
  char xmlPath[1000] = {'\0'};
  const char* path = ACE_OS::getenv("TESTSRC");
  ASSERT(path != nullptr,
         "Environment variable TESTSRC for test source directory is not set.");
  strncpy(xmlPath, path, strlen(path) - strlen("cppcache"));
  strcat(xmlPath, "xml/Security/");
  return std::string(xmlPath);
}

void initCredentialGenerator() {
  static int loopNum = 1;

  switch (loopNum) {
    case 1: {
      credentialGeneratorHandler = CredentialGenerator::create("DUMMY");
      break;
    }
    case 2: {
      credentialGeneratorHandler = CredentialGenerator::create("LDAP");
      break;
    }
    default:
    case 3: {
      credentialGeneratorHandler = CredentialGenerator::create("PKCS");
      break;
    }
  }

  if (credentialGeneratorHandler == nullptr) {
    FAIL("credentialGeneratorHandler is nullptr");
  }

  loopNum++;
  if (loopNum > 3) loopNum = 1;
}

opCodeList::value_type tmpRArr[] = {
    OP_GET,     OP_GETALL,      OP_REGISTER_INTEREST, OP_UNREGISTER_INTEREST,
    OP_KEY_SET, OP_CONTAINS_KEY};

opCodeList::value_type tmpWArr[] = {OP_CREATE,  OP_UPDATE,     OP_PUTALL,
                                    OP_DESTROY, OP_INVALIDATE, OP_REGION_CLEAR};

opCodeList::value_type tmpAArr[] = {OP_CREATE,       OP_UPDATE,
                                    OP_DESTROY,      OP_INVALIDATE,
                                    OP_REGION_CLEAR, OP_REGISTER_INTEREST,
                                    OP_GET,          OP_QUERY,
                                    OP_REGISTER_CQ,  OP_EXECUTE_FUNCTION};

#define HANDLE_NO_NOT_AUTHORIZED_EXCEPTION                       \
  catch (const apache::geode::client::NotAuthorizedException&) { \
    LOG("NotAuthorizedException Caught");                        \
    FAIL("should not have caught NotAuthorizedException");       \
  }                                                              \
  catch (const apache::geode::client::Exception& other) {        \
    LOG("Got apache::geode::client::Exception& other ");         \
    LOG(other.getStackTrace().c_str());                          \
    FAIL(other.what());                                          \
  }

#define HANDLE_NOT_AUTHORIZED_EXCEPTION                          \
  catch (const apache::geode::client::NotAuthorizedException&) { \
    LOG("NotAuthorizedException Caught");                        \
    LOG("Success");                                              \
  }                                                              \
  catch (const apache::geode::client::Exception& other) {        \
    LOG(other.getStackTrace().c_str());                          \
    FAIL(other.what());                                          \
  }

#define ADMIN_CLIENT s1p1
#define WRITER_CLIENT s1p2
#define READER_CLIENT s2p1
//#define USER_CLIENT s2p2

#define TYPE_ADMIN_CLIENT 'A'
#define TYPE_WRITER_CLIENT 'W'
#define TYPE_READER_CLIENT 'R'
#define TYPE_USER_CLIENT 'U'

const char* regionNamesAuth[] = {"DistRegionAck"};

void initClientAuth(char UserType) {
  auto config = Properties::create();
  opCodeList wr(tmpWArr, tmpWArr + sizeof tmpWArr / sizeof *tmpWArr);
  opCodeList rt(tmpRArr, tmpRArr + sizeof tmpRArr / sizeof *tmpRArr);
  opCodeList ad(tmpAArr, tmpAArr + sizeof tmpAArr / sizeof *tmpAArr);
  credentialGeneratorHandler->getAuthInit(config);
  switch (UserType) {
    case 'W':
      credentialGeneratorHandler->getAllowedCredentialsForOps(wr, config,
                                                              nullptr);
      break;
    case 'R':
      credentialGeneratorHandler->getAllowedCredentialsForOps(rt, config,
                                                              nullptr);
      break;
    case 'A':
      credentialGeneratorHandler->getAllowedCredentialsForOps(ad, config,
                                                              nullptr);
    default:
      break;
  }

  auto alias = config->find("security-alias");
  auto uname = config->find("security-username");
  auto passwd = config->find("security-password");

  char msgAlias[100];
  char msgUname[100];
  char msgPasswd[100];

  sprintf(msgAlias, "PKCS alias is %s",
          alias == nullptr ? "null" : alias->value().c_str());
  sprintf(msgUname, "username is %s",
          uname == nullptr ? "null" : uname->value().c_str());
  sprintf(msgPasswd, "password is %s",
          passwd == nullptr ? "null" : passwd->value().c_str());

  LOG(msgAlias);
  LOG(msgUname);
  LOG(msgPasswd);

  config->insert("ssl-enabled", "true");
  std::string keystore = std::string(ACE_OS::getenv("TESTSRC")) + "/keystore";
  std::string pubkey = keystore + "/client_truststore.pem";
  std::string privkey = keystore + "/client_keystore.pem";
  config->insert("ssl-keystore", privkey.c_str());
  config->insert("ssl-truststore", pubkey.c_str());

  try {
    initClient(true, config);
  } catch (...) {
    throw;
  }
}

DUNIT_TASK_DEFINITION(ADMIN_CLIENT, StartServer1_With_SSL)
  {
    initCredentialGenerator();
    std::string cmdServerAuthenticator;

    if (isLocalServer) {
      cmdServerAuthenticator = credentialGeneratorHandler->getServerCmdParams(
          "authenticator:authorizer", getXmlPath());
      printf("string %s", cmdServerAuthenticator.c_str());
      CacheHelper::initServer(
          1, "cacheserver_notify_subscription.xml", locHostPort,
          const_cast<char*>(cmdServerAuthenticator.c_str()), true);
      LOG("Server1 started");
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(ADMIN_CLIENT, StartServer2_With_SSL)
  {
    std::string cmdServerAuthenticator;

    if (isLocalServer) {
      cmdServerAuthenticator = credentialGeneratorHandler->getServerCmdParams(
          "authenticator:authorizer", getXmlPath());
      printf("string %s", cmdServerAuthenticator.c_str());
      CacheHelper::initServer(
          2, "cacheserver_notify_subscription2.xml", locHostPort,
          const_cast<char*>(cmdServerAuthenticator.c_str()), true);
      LOG("Server2 started");
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(ADMIN_CLIENT, StartLocator_With_SSL)
  {
    if (isLocator) {
      CacheHelper::initLocator(1, true);
      LOG("Locator1 started");
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(ADMIN_CLIENT, StepOne)
  {
    initClientAuth('A');
    try {
      LOG("Tying Region creation");
      createRegionForSecurity(regionNamesAuth[0], USE_ACK, true);
      LOG("Region created successfully");
      LOG("Tying Entry creation");
      createEntry(regionNamesAuth[0], keys[0], vals[0]);
      LOG("Entry created successfully");
      updateEntry(regionNamesAuth[0], keys[0], nvals[0]);
      LOG("Entry updated successfully");
      HashMapOfCacheable entrymap;
      entrymap.clear();
      for (int i = 0; i < 5; i++) {
        entrymap.emplace(CacheableKey::create(i), CacheableInt32::create(i));
      }
      auto regPtr = getHelper()->getRegion(regionNamesAuth[0]);
      regPtr->putAll(entrymap);
      LOG("PutAll completed successfully");
      for (int i = 0; i < 5; i++) {
        regPtr->invalidate(CacheableKey::create(i));
      }
      std::vector<std::shared_ptr<CacheableKey>> entrykeys;
      for (int i = 0; i < 5; i++) {
        entrykeys.push_back(CacheableKey::create(i));
      }
      const auto values = regPtr->getAll(entrykeys);
      if (values.size() > 0) {
        LOG("GetAll completed successfully");
      } else {
        FAIL("GetAll did not complete successfully");
      }
      regPtr->query("1=1");
      LOG("Query completed successfully");
      auto pool =
          getHelper()->getCache()->getPoolManager().find(regionNamesAuth[0]);
      std::shared_ptr<QueryService> qs;
      if (pool != nullptr) {
        // Using region name as pool name
        qs = pool->getQueryService();
      } else {
        qs = getHelper()->cachePtr->getQueryService();
      }
      char queryString[100];
      sprintf(queryString, "select * from /%s", regionNamesAuth[0]);
      CqAttributesFactory cqFac;
      auto cqAttrs = cqFac.create();
      auto qry = qs->newCq("cq_security", queryString, cqAttrs);
      qs->executeCqs();
      LOG("CQ completed successfully");
      if (pool != nullptr) {
        FunctionService::onServer(pool)->execute("securityTest")->getResult();
        LOG("Function execution completed successfully");
      } else {
        LOG("Skipping function execution for non pool case");
      }
      invalidateEntry(regionNamesAuth[0], keys[0]);
      LOG("Entry invalidated successfully");
      verifyInvalid(regionNamesAuth[0], keys[0]);
      LOG("Entry invalidate-verified successfully");
      destroyEntry(regionNamesAuth[0], keys[0]);
      LOG("Entry destroyed successfully");
      verifyDestroyed(regionNamesAuth[0], keys[0]);
      LOG("Entry destroy-verified successfully");
      destroyRegion(regionNamesAuth[0]);
      LOG("Region destroy successfully");
      LOG("Tying Region creation");
      createRegionForSecurity(regionNamesAuth[0], USE_ACK, true);
      LOG("Region created successfully");
      createEntry(regionNamesAuth[0], keys[2], vals[2]);
      LOG("Entry created successfully");
      auto regPtr0 = getHelper()->getRegion(regionNamesAuth[0]);
      if (regPtr0 != nullptr) {
        LOG("Going to do registerAllKeys");
        regPtr0->registerAllKeys();
        LOG("Going to do unregisterAllKeys");
        regPtr0->unregisterAllKeys();
      }
    }
    HANDLE_NO_NOT_AUTHORIZED_EXCEPTION
    LOG("StepOne complete.");
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(WRITER_CLIENT, StepTwo)
  {
    initCredentialGenerator();
    initClientAuth('W');
    try {
      createRegionForSecurity(regionNamesAuth[0], USE_ACK, true);
      LOG("Region created successfully");
      createEntry(regionNamesAuth[0], keys[0], vals[0]);
      LOG("Entry created successfully");
      updateEntry(regionNamesAuth[0], keys[0], nvals[0]);
      LOG("Entry updated successfully");
      HashMapOfCacheable entrymap;
      entrymap.clear();
      for (int i = 0; i < 5; i++) {
        entrymap.emplace(CacheableKey::create(i), CacheableInt32::create(i));
      }
      auto regPtr = getHelper()->getRegion(regionNamesAuth[0]);
      regPtr->putAll(entrymap);
      LOG("PutAll completed successfully");
      invalidateEntry(regionNamesAuth[0], keys[0]);
      LOG("Entry invalidated successfully");
      verifyInvalid(regionNamesAuth[0], keys[0]);
      LOG("Entry invalidate-verified successfully");
      destroyEntry(regionNamesAuth[0], keys[0]);
      LOG("Entry destroyed successfully");
      verifyDestroyed(regionNamesAuth[0], keys[0]);
      LOG("Entry destroy-verified successfully");
      createEntry(regionNamesAuth[0], keys[0], vals[0]);
      LOG("Entry created successfully");
      updateEntry(regionNamesAuth[0], keys[0], nvals[0]);
      LOG("Entry updated successfully");
      verifyEntry(regionNamesAuth[0], keys[0], nvals[0]);
      LOG("Entry updation-verified successfully");
    }
    HANDLE_NO_NOT_AUTHORIZED_EXCEPTION
    try {
      auto regPtr0 = getHelper()->getRegion(regionNamesAuth[0]);
      auto keyPtr = CacheableKey::create(keys[2]);
      auto checkPtr =
          std::dynamic_pointer_cast<CacheableString>(regPtr0->get(keyPtr));
      if (checkPtr != nullptr) {
        char buf[1024];
        sprintf(buf, "In net search, get returned %s for key %s",
                checkPtr->value().c_str(), keys[2]);
        LOG(buf);
        FAIL("Should not get the value");
      } else {
        LOG("checkPtr is nullptr");
      }
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION
    auto regPtr0 = getHelper()->getRegion(regionNamesAuth[0]);
    try {
      LOG("Going to do registerAllKeys");
      regPtr0->registerAllKeys();
      FAIL("Should not be able to do Register Interest");
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    try {
      for (int i = 0; i < 5; i++) {
        regPtr0->invalidate(CacheableKey::create(i));
      }
      std::vector<std::shared_ptr<CacheableKey>> entrykeys;
      for (int i = 0; i < 5; i++) {
        entrykeys.push_back(CacheableKey::create(i));
      }

      const auto values = regPtr0->getAll(entrykeys);
      if (values.size() > 0) {
        FAIL("GetAll should not have completed successfully");
      }
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    try {
      regPtr0->query("1=1");
      FAIL("Query should not have completed successfully");
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    auto pool =
        getHelper()->getCache()->getPoolManager().find(regionNamesAuth[0]);

    try {
      std::shared_ptr<QueryService> qs;
      if (pool != nullptr) {
        // Using region name as pool name
        qs = pool->getQueryService();
      } else {
        qs = getHelper()->cachePtr->getQueryService();
      }
      char queryString[100];
      sprintf(queryString, "select * from /%s", regionNamesAuth[0]);
      CqAttributesFactory cqFac;
      auto cqAttrs = cqFac.create();
      auto qry = qs->newCq("cq_security", queryString, cqAttrs);
      qs->executeCqs();
      FAIL("CQ should not have completed successfully");
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    try {
      if (pool != nullptr) {
        FunctionService::onServer(pool)->execute("securityTest")->getResult();
        //   FAIL("Function execution should not have completed successfully");
      } else {
        LOG("Skipping function execution for non pool case");
      }
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    createEntry(regionNamesAuth[0], keys[2], vals[2]);
    LOG("Entry created successfully");

    LOG("StepTwo complete.");
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(READER_CLIENT, StepThree)
  {
    initCredentialGenerator();
    initClientAuth('R');
    std::shared_ptr<Region> rptr;
    char buf[100];
    int i = 102;

    createRegionForSecurity(regionNamesAuth[0], USE_ACK, true);

    rptr = getHelper()->getRegion(regionNamesAuth[0]);
    sprintf(buf, "%s: %d", rptr->getName().c_str(), i);
    auto key = createKey(buf);
    sprintf(buf, "testUpdate::%s: value of %d", rptr->getName().c_str(), i);
    auto valuePtr = buf;
    try {
      LOG("Trying put Operation");
      rptr->put(key, valuePtr);
      LOG(" Put Operation Successful");
      FAIL("Should have got NotAuthorizedException during put");
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    try {
      LOG("Trying createEntry");
      createEntry(regionNamesAuth[0], keys[2], vals[2]);
      FAIL("Should have got NotAuthorizedException during createEntry");
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    ASSERT(!rptr->containsKey(keys[2]),
           "Key should not have been found in the region");

    try {
      LOG("Trying updateEntry");
      updateEntry(regionNamesAuth[0], keys[2], nvals[2], false, false);
      FAIL("Should have got NotAuthorizedException during updateEntry");
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    ASSERT(!rptr->containsKey(keys[2]),
           "Key should not have been found in the region");

    try {
      auto regPtr0 = getHelper()->getRegion(regionNamesAuth[0]);
      auto keyPtr = CacheableKey::create(keys[2]);
      auto checkPtr =
          std::dynamic_pointer_cast<CacheableString>(regPtr0->get(keyPtr));
      if (checkPtr != nullptr) {
        char buf[1024];
        sprintf(buf, "In net search, get returned %s for key %s",
                checkPtr->value().c_str(), keys[2]);
        LOG(buf);
      } else {
        LOG("checkPtr is nullptr");
      }
    }
    HANDLE_NO_NOT_AUTHORIZED_EXCEPTION

    auto regPtr0 = getHelper()->getRegion(regionNamesAuth[0]);
    if (regPtr0 != nullptr) {
      try {
        LOG("Going to do registerAllKeys");
        regPtr0->registerAllKeys();
        LOG("Going to do unregisterAllKeys");
        regPtr0->unregisterAllKeys();
      }
      HANDLE_NO_NOT_AUTHORIZED_EXCEPTION
    }

    try {
      HashMapOfCacheable entrymap;
      entrymap.clear();
      for (int i = 0; i < 5; i++) {
        entrymap.emplace(CacheableKey::create(i), CacheableInt32::create(i));
      }
      regPtr0->putAll(entrymap);
      FAIL("PutAll should not have completed successfully");
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    try {
      std::vector<std::shared_ptr<CacheableKey>> entrykeys;
      for (int i = 0; i < 5; i++) {
        entrykeys.push_back(CacheableKey::create(i));
      }

      const auto values = regPtr0->getAll(entrykeys);
      if (values.size() > 0) {
        LOG("GetAll completed successfully");
      } else {
        FAIL("GetAll did not complete successfully");
      }
    }
    HANDLE_NO_NOT_AUTHORIZED_EXCEPTION

    try {
      regPtr0->query("1=1");
      FAIL("Query should not have completed successfully");
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    auto pool =
        getHelper()->getCache()->getPoolManager().find(regionNamesAuth[0]);

    try {
      std::shared_ptr<QueryService> qs;
      if (pool != nullptr) {
        // Using region name as pool name
        qs = pool->getQueryService();
      } else {
        qs = getHelper()->cachePtr->getQueryService();
      }
      char queryString[100];
      sprintf(queryString, "select * from /%s", regionNamesAuth[0]);
      CqAttributesFactory cqFac;
      auto cqAttrs = cqFac.create();
      auto qry = qs->newCq("cq_security", queryString, cqAttrs);
      qs->executeCqs();
      //    FAIL("CQ should not have completed successfully");
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    try {
      if (pool != nullptr) {
        FunctionService::onServer(pool)->execute("securityTest")->getResult();
        FAIL("Function execution should not have completed successfully");
      } else {
        LOG("Skipping function execution for non pool case");
      }
    }
    HANDLE_NOT_AUTHORIZED_EXCEPTION

    LOG("StepThree complete.");
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(ADMIN_CLIENT, CloseServer1)
  {
    SLEEP(9000);
    if (isLocalServer) {
      CacheHelper::closeServer(1);
      LOG("SERVER1 stopped");
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(ADMIN_CLIENT, CloseServer2)
  {
    if (isLocalServer) {
      CacheHelper::closeServer(2);
      LOG("SERVER2 stopped");
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(ADMIN_CLIENT, CloseLocator_With_SSL)
  {
    if (isLocator) {
      CacheHelper::closeLocator(1, true);
      LOG("Locator1 stopped");
    }
  }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(ADMIN_CLIENT, CloseCacheAdmin)
  { cleanProc(); }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(WRITER_CLIENT, CloseCacheWriter)
  { cleanProc(); }
END_TASK_DEFINITION

DUNIT_TASK_DEFINITION(READER_CLIENT, CloseCacheReader)
  { cleanProc(); }
END_TASK_DEFINITION

void doThinClientSSLWithSecurityAuthorization() {
  CALL_TASK(StartLocator_With_SSL);
  CALL_TASK(StartServer1_With_SSL);

  CALL_TASK(StepOne);
  CALL_TASK(StepTwo);
  CALL_TASK(StartServer2_With_SSL);
  CALL_TASK(CloseServer1);
  CALL_TASK(StepThree);
  CALL_TASK(CloseCacheReader);
  CALL_TASK(CloseCacheWriter);
  CALL_TASK(CloseCacheAdmin);
  CALL_TASK(CloseServer2);

  CALL_TASK(CloseLocator_With_SSL);
}

DUNIT_MAIN
  { doThinClientSSLWithSecurityAuthorization(); }
END_MAIN
