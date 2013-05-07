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

package es.tid.cosmos.servicemanager.ambari.rest

import scala.concurrent.Future

import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json.JsonAST._

/**
 * Root class that enables REST calls to the Ambari server.
 *
 * @constructor
 * @param serverUrl the url of the server
 * @param port      the port where the server is listening to REST calls
 * @param username  the username used for authentication
 * @param password  the password used for authentication
 */
class AmbariServer(serverUrl: String, port: Int, username: String, password: String)
  extends ClusterProvisioner with JsonHttpRequest {

  private[this] def baseUrl = host(serverUrl, port).as_!(username, password) / "api" / "v1"

  override def listClusterNames: Future[Seq[String]] =
    performRequest(baseUrl / "clusters").map(json => for {
      JField("cluster_name", JString(name)) <- (json \\ "cluster_name").children
    } yield name)

  override def getCluster(name: String): Future[Cluster] =
    performRequest(baseUrl / "clusters" / name).map(new Cluster(_, baseUrl.build))

  override def createCluster(name: String, version: String): Future[Cluster] =
    performRequest(baseUrl / "clusters" / name << s"""{"Clusters": {"version": "$version"}}""")
      .flatMap(_ => getCluster(name))

  override def removeCluster(name: String): Future[JValue] =
    performRequest(baseUrl.DELETE / "clusters" / name)
}
