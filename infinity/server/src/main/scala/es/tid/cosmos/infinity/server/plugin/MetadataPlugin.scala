/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.plugin

import org.apache.commons.logging.LogFactory
import org.apache.hadoop.conf.{Configurable, Configuration}
import org.apache.hadoop.hdfs.server.namenode.NameNode
import org.apache.hadoop.util.ServicePlugin

import es.tid.cosmos.infinity.server.authentication.AuthenticationComponent
import es.tid.cosmos.infinity.server.config.InfinityConfig
import es.tid.cosmos.infinity.server.finatra.{FinatraUrlMapper, MetadataServer}
import es.tid.cosmos.infinity.server.actions.hdfs.HdfsNameNode

/** Namenode plugin to serve Infinity metadata. */
class MetadataPlugin extends ServicePlugin with Configurable {
  
  this: AuthenticationComponent =>

  private val log = LogFactory.getLog(classOf[MetadataPlugin])
  private var hadoopConfOpt: Option[Configuration] = None
  private var serverOpt: Option[MetadataServer] = None

  override def setConf(conf: Configuration): Unit = {
    hadoopConfOpt = Some(conf)
  }

  override def getConf: Configuration =
    hadoopConfOpt.getOrElse(throw new IllegalStateException("Not yet injected with Hadoop config"))

  override def start(service: Any): Unit = service match {
    case nameNode: NameNode =>
      while (nameNode.isInSafeMode) {
        log.info("Waiting for NN to exit safe mode before starting metadata server")
        Thread.sleep(1000)
      }
      log.info("Starting Infinity metadata server as a namenode plugin")
      val config = new InfinityConfig(PluginConfig.load(getConf))
      val urlMapper = new FinatraUrlMapper(config)
      val server = new MetadataServer(
        nameNode = new HdfsNameNode(config, nameNode.getRpcServer, urlMapper),
        config = config,
        authService = authentication)
      server.start()
      serverOpt = Some(server)
    case other =>
      log.error(
        s"""Metadata plugin initialization failed: a NameNode was expected but ${service.getClass}
          | was found. Make sure you have configured it as namenode plugin instead of datanode one.
        """.stripMargin
      )
  }

  override def stop(): Unit = {
    log.info("Shutting down Infinity metadata plugin")
    serverOpt.foreach(_.shutdown())
    serverOpt = None
  }

  override def close(): Unit = stop()
}
