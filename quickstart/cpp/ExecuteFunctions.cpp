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
 * The Execute Function QuickStart Example.
 *
 * This example takes the following steps:
 *
 * 1. Create a Geode Cache.
 * 2. Create the example Region Programmatically.
 * 3. Populate some objects on the Region.
 * 4. Create Execute Objects
 * 5. Execute Functions
 * 6. Close the Cache.
 *
 */

// Include the Geode library.

// Use the "geode" namespace.
using namespace apache::geode::client;

char* getFuncIName = (char*)"MultiGetFunctionI";
char* putFuncIName = (char*)"MultiPutFunctionI";
char* getFuncName = (char*)"MultiGetFunction";
char* putFuncName = (char*)"MultiPutFunction";

// The Execute Function QuickStart example.
int main(int argc, char** argv) {
  try {
    // Create CacheFactory using the settings from the geode.properties file by
    // default.
    auto cacheFactory = CacheFactory::createCacheFactory();
    LOGINFO("Created CacheFactory");

    // Create a Geode Cache.
    auto cachePtr = cacheFactory->setSubscriptionEnabled(true)
                        ->addServer("localhost", 50505)
                        ->addServer("localhost", 40404)
                        ->create();
    LOGINFO("Created the Geode Cache");

    // Create the example Region Programmatically
    auto regPtr0 = cachePtr->createRegionFactory(CACHING_PROXY)
                       ->create("partition_region");
    LOGINFO("Created the Region");

    regPtr0->registerAllKeys();
    char buf[128];

    auto resultList = CacheableVector::create();
    for (int i = 0; i < 34; i++) {
      sprintf(buf, "VALUE--%d", i);
      std::shared_ptr<Cacheable> value(buf);

      sprintf(buf, "KEY--%d", i);
      auto key = CacheableKey::create(buf);
      regPtr0->put(key, value);
    }

    bool getResult = true;
    auto routingObj = CacheableVector::create();
    for (int i = 0; i < 34; i++) {
      if (i % 2 == 0) continue;
      sprintf(buf, "KEY--%d", i);
      auto key = CacheableKey::create(buf);
      routingObj->push_back(key);
    }

    LOGINFO("test data independent function with result on one server");
    auto args = routingObj;
    auto exc =
        FunctionService::onServer((std::shared_ptr<RegionService>)cachePtr);
    auto executeFunctionResult =
        exc->withArgs(args)->execute(getFuncIName)->getResult();
    if (executeFunctionResult == nullptr) {
      LOGINFO("get executeFunctionResult is NULL");
    } else {
      for (int32_t item = 0; item < executeFunctionResult->size(); item++) {
        auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
            executeFunctionResult->operator[](item));
        for (int32_t pos = 0; pos < arrayList->size(); pos++) {
          resultList->push_back(arrayList->operator[](pos));
        }
      }
      sprintf(buf, "get: result count = %d", resultList->size());
      LOGINFO(buf);
      for (int32_t i = 0; i < executeFunctionResult->size(); i++) {
        sprintf(buf, "get result[%d]=%s", i,
                std::dynamic_pointer_cast<CacheableString>(
                    resultList->operator[](i))
                    ->value()
                    .c_str());
        LOGINFO(buf);
      }
    }

    LOGINFO("test data independent function without result on one server");
    getResult = false;
    exc->withArgs(args)->execute(putFuncIName, 15);

    LOGINFO("test data independent function with result on all servers");
    getResult = true;
    exc = FunctionService::onServers((std::shared_ptr<RegionService>)cachePtr);
    executeFunctionResult =
        exc->withArgs(args)->execute(getFuncIName)->getResult();
    if (executeFunctionResult == nullptr) {
      LOGINFO("get executeFunctionResult is NULL");
    } else {
      resultList->clear();
      for (int32_t item = 0; item < executeFunctionResult->size(); item++) {
        auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
            executeFunctionResult->operator[](item));
        for (int32_t pos = 0; pos < arrayList->size(); pos++) {
          resultList->push_back(arrayList->operator[](pos));
        }
      }
      sprintf(buf, "get: result count = %d", resultList->size());
      LOGINFO(buf);
      for (int32_t i = 0; i < executeFunctionResult->size(); i++) {
        sprintf(buf, "get result[%d]=%s", i,
                std::dynamic_pointer_cast<CacheableString>(
                    resultList->operator[](i))
                    ->value()
                    .c_str());
        LOGINFO(buf);
      }
    }

    getResult = false;
    LOGINFO("test data independent function without result on all servers");
    exc->withArgs(args)->execute(putFuncIName, 15);
    LOGINFO("test data dependent function with result");
    getResult = true;
    args = CacheableBoolean::create(1);
    exc = FunctionService::onRegion(regPtr0);
    executeFunctionResult = exc->withFilter(routingObj)
                                ->withArgs(args)
                                ->execute(getFuncName)
                                ->getResult();
    if (executeFunctionResult == nullptr) {
      LOGINFO("execute on region: executeFunctionResult is NULL");
    } else {
      resultList->clear();
      LOGINFO("Execute on Region: result count = %d",
              executeFunctionResult->size());
      for (int32_t i = 0; i < executeFunctionResult->size(); i++) {
        auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
            executeFunctionResult->operator[](i));
        for (int32_t pos = 0; pos < arrayList->size(); pos++) {
          resultList->push_back(arrayList->operator[](pos));
        }
      }
      sprintf(buf, "Execute on Region: result count = %d", resultList->size());
      LOGINFO(buf);

      for (int32_t i = 0; i < resultList->size(); i++) {
        sprintf(buf, "Execute on Region: result[%d]=%s", i,
                std::dynamic_pointer_cast<CacheableString>(
                    resultList->operator[](i))
                    ->value()
                    .c_str());
        LOGINFO(buf);
      }
    }

    LOGINFO("test data dependent function without result");
    getResult = false;
    exc->withFilter(routingObj)->withArgs(args)->execute(putFuncName, 15);

    // Close the Geode Cache.
    cachePtr->close();
    LOGINFO("Closed the Geode Cache");

    return 0;
  }
  // An exception should not occur
  catch (const Exception& geodeExcp) {
    LOGERROR("Function Execution Geode Exception: %s", geodeExcp.getMessage());

    return 1;
  }
}
