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

using System;
using System.Collections.Generic;
using System.IO;
using Xunit;
using Xunit.Abstractions;

namespace Apache.Geode.Client.IntegrationTests
{
    [Trait("Category", "Integration")]
    public class RegionSSLTest : TestBase, IDisposable
    {
        public RegionSSLTest(ITestOutputHelper testOutputHelper) : base(testOutputHelper)
        {
        }

        public void Dispose()
        {
        }

        public Cache CreateCache(string keystore, string keystorePassword, string truststore)
        {
            var cacheFactory = new CacheFactory();
            cacheFactory.Set("log-level", "none");
            cacheFactory.Set("ssl-enabled", "true");
            cacheFactory.Set("ssl-keystore", keystore);
            cacheFactory.Set("ssl-keystore-password", keystorePassword);
            cacheFactory.Set("ssl-truststore", truststore);

            return cacheFactory.Create();
        }

        [Fact]
        public void SslPutGetTest()
        {
            using (var cluster = new Cluster(output, CreateTestCaseDirectoryName(), 1, 1))
            {
                cluster.UseSsl(Environment.CurrentDirectory + @"\ServerSslKeys\server_keystore.jks",
                    "gemstone",
                    Environment.CurrentDirectory + @"\ServerSslKeys\server_truststore.jks",
                    "gemstone");
                Assert.True(cluster.Start());
                Assert.Equal(0, cluster.Gfsh
                    .create()
                    .region()
                    .withName("testRegion1")
                    .withType("PARTITION")
                    .execute());

                var cache = cluster.CreateCache(new Dictionary<string, string>(){ { "ssl-enabled", "true" },
                    { "ssl-keystore", Environment.CurrentDirectory + @"\ClientSslKeys\client_keystore.password.pem" },
                    { "ssl-keystore-password", "gemstone" },
                    { "ssl-truststore", Environment.CurrentDirectory + @"\ClientSslKeys\client_keystore.password.pem" } });

                var regionFactory = cache.CreateRegionFactory(RegionShortcut.PROXY)
                            .SetPoolName("default");

                var region = regionFactory.Create<string, string>("testRegion1");

                const string key = "hello";
                const string expectedResult = "dave";

                region.Put(key, expectedResult);
                var actualResult = region.Get(key);

                Assert.Equal(expectedResult, actualResult);

                cache.Close();
            }
        }

        [Fact]
        public void SslChainedCertificatePutGetTest()
        {
            using (var cluster = new Cluster(output, CreateTestCaseDirectoryName(), 1, 1))
            {
                cluster.UseSsl(Environment.CurrentDirectory + @"\ServerSslKeys\server_keystore_chained.p12",
                    "apachegeode",
                    Environment.CurrentDirectory + @"\ServerSslKeys\server_truststore_chained_root.jks",
                    "apachegeode");
                Assert.True(cluster.Start());
                Assert.Equal(0, cluster.Gfsh
                    .create()
                    .region()
                    .withName("testRegion1")
                    .withType("PARTITION")
                    .execute());

                var cache = CreateCache(Environment.CurrentDirectory + @"\ClientSslKeys\client_keystore_chained.pem",
                    "apachegeode",
                    Environment.CurrentDirectory + @"\ClientSslKeys\client_truststore_chained_root.pem");

                cluster.ApplyLocators(cache.GetPoolFactory()).Create("default");

                var regionFactory = cache.CreateRegionFactory(RegionShortcut.PROXY)
                            .SetPoolName("default");

                var region = regionFactory.Create<string, string>("testRegion1");

                const string key = "hello";
                const string expectedResult = "dave";

                region.Put(key, expectedResult);
                var actualResult = region.Get(key);

                Assert.Equal(expectedResult, actualResult);

                cache.Close();
            }
        }
    }
}
