/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.infinity.server

import com.typesafe.config.{Config, ConfigFactory}
import org.apache.commons.logging.LogFactory
import org.apache.hadoop.conf.{Configurable, Configuration}
import org.apache.hadoop.hdfs.server.namenode.NameNode
import org.apache.hadoop.util.ServicePlugin

/** Namenode plugin to serve Infinity metadada. */
class MetadataPlugin extends ServicePlugin with Configurable {

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
      log.info("Starting Infinity metadata server as a namenode plugin")
      val server = new MetadataServer(nameNode, metadataServerConfig)
      server.start()
      serverOpt = Some(server)
    case other =>
      log.error(
        s"""Metadata plugin initialization failed: a NameNode was expected but ${service.getClass}
          | was found. Make sure you have configured it as namenode plugin instead of datanode one.
        """.stripMargin
      )
  }

  private def metadataServerConfig: Config = {
    // TODO: use hadoop config at least for overrides
    ConfigFactory.load()
  }

  override def stop(): Unit = {
    log.info("Shutting down Infinity metadata plugin")
    serverOpt.foreach(_.shutdown())
    serverOpt = None
  }

  override def close(): Unit = stop()
}
