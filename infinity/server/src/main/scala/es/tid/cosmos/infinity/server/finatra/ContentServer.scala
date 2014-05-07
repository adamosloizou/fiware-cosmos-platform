/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.finatra

import org.apache.hadoop.hdfs.server.datanode.DataNode

import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.config.InfinityContentServerConfig

class ContentServer(
    dataNode: DataNode,
    config: InfinityContentServerConfig,
    authService: AuthenticationService) {

  val serverConfig = FinatraServerCfg(
    http = Some(s"0.0.0.0:${config.contentServerUrl.getPort}")
  )
  val urlMapper = new FinatraUrlMapper(config)

  val server = new EmbeddableFinatraServer(serverConfig)

  server.register(new ContentRoutes(config, authService, dataNode, urlMapper))

  def start(): Unit = server.start()

  def shutdown(): Unit = server.stop()

}
