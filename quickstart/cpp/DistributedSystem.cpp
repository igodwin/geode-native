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
 * The DistributedSystem QuickStart Example.
 * This example creates two pools programatically.
 * This example takes the following steps:
 *
 * 1. Create a Geode Cache.
 * 2. Now it creates Pool with poolName1.
 * 3. Adds server(localhost:40404) to pool factory.
 * 4. Creates region "root1" with pool.
 * 5. Put Entries (Key and Value pairs) into the Region.
 * 6. Get Entries from the Region.
 * 7. Invalidate an Entry in the Region.
 * 8. Destroy an Entry in the Region.
 * 9. Now it creates another reagion "root2" with another pool "poolName2",
 * which connects to the server(localhost:40405).
 * 10. Now it do put/get operations.
 * 10. Close the Cache.
 *
 */

// Include the Geode library.

// Use the "geode" namespace.
using namespace apache::geode::client;

void distributedsystem(std::shared_ptr<Cache> cachePtr, char *hostname,
                       int port, char *poolName, char *regionName) {
  // create pool factory to create the pool.
  auto poolFacPtr = cachePtr->getPoolManager().createFactory();

  // adding host(endpoint) in pool
  poolFacPtr->addServer(hostname, port);

  // enabling subscription on pool
  poolFacPtr->setSubscriptionEnabled(true);

  // creating pool with name "examplePool"
  poolFacPtr->create(poolName, *cachePtr.get());

  auto regionFactory = cachePtr->createRegionFactory(CACHING_PROXY);

  LOGINFO("Created the RegionFactory");

  // setting pool to attach with region
  regionFactory->setPoolName(poolName);

  // creating first root region
  auto regionPtr = regionFactory->create(regionName);

  LOGINFO("Created Region.");

  // Put an Entry (Key and Value pair) into the Region using the direct/shortcut
  // method.
  regionPtr->put("Key1", "Value1");

  LOGINFO("Put the first Entry into the Region");

  // Put an Entry into the Region by manually creating a Key and a Value pair.
  auto keyPtr = CacheableInt32::create(123);
  auto valuePtr = CacheableString::create("123");
  regionPtr->put(keyPtr, valuePtr);

  LOGINFO("Put the second Entry into the Region");

  // Get Entries back out of the Region.
  auto result1Ptr = regionPtr->get("Key1");

  LOGINFO("Obtained the first Entry from the Region");

  auto result2Ptr = regionPtr->get(keyPtr);

  LOGINFO("Obtained the second Entry from the Region");

  // Invalidate an Entry in the Region.
  regionPtr->invalidate("Key1");

  LOGINFO("Invalidated the first Entry in the Region");

  // Destroy an Entry in the Region.
  regionPtr->destroy(keyPtr);

  LOGINFO("Destroyed the second Entry in the Region");
}

// The DistributedSystem QuickStart example.
int main(int argc, char **argv) {
  try {
    auto cacheFactory = CacheFactory::createCacheFactory();

    LOGINFO("Connected to the Geode Distributed System");

    // Create a Geode Cache.
    auto cachePtr = cacheFactory->create();

    LOGINFO("Created the Geode Cache");

    distributedsystem(cachePtr, (char *)"localhost", 40404, (char *)"poolName1",
                      (char *)"exampleRegion1");
    distributedsystem(cachePtr, (char *)"localhost", 40405, (char *)"poolName2",
                      (char *)"exampleRegion2");

    // Close the Geode Cache.
    cachePtr->close();

    LOGINFO("Closed the Geode Cache");

    return 0;
  }
  // An exception should not occur
  catch (const Exception &geodeExcp) {
    LOGERROR("DistributedSystem Geode Exception: %s", geodeExcp.getMessage());

    return 1;
  }
}
