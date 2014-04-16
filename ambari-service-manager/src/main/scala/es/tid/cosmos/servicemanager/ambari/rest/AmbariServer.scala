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

package es.tid.cosmos.servicemanager.ambari.rest

import scala.concurrent.Future

import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json._
import net.liftweb.json.JsonDSL._
import net.liftweb.json.JsonAST.JString
import net.liftweb.json.render

import es.tid.cosmos.common.SequentialOperations
import es.tid.cosmos.servicemanager.{ServiceError, RequestException}

/** Root class that enables REST calls to the Ambari server.
  *
  * @constructor
  * @param serverUrl the url of the server
  * @param port      the port where the server is listening to REST calls
  * @param username  the username used for authentication
  * @param password  the password used for authentication
  */
private[ambari] class AmbariServer(serverUrl: String, port: Int, username: String, password: String)
  extends RequestProcessor with BootstrapRequestHandlerFactory {
  implicit private val formats = DefaultFormats

  private[this] def baseUrl = host(serverUrl, port).as_!(username, password) / "api" / "v1"

  /** Get a list of the names of the existing, managed clusters.
    *
    * @return the future of the list of names
    */
  def listClusterNames: Future[Seq[String]] =
    performRequest(baseUrl / "clusters").map(json => as.FlatValues(json, "items", "cluster_name"))

  /** Get the cluster specified by the given name.
    *
    * @param id the id of the cluster
    * @return the future of the cluster iff found
    */
  def getCluster(id: String): Future[Cluster] =
    performRequest(baseUrl / "clusters" / id).map(new Cluster(_, baseUrl.build))

  /** Create a cluster.
    *
    * @param name the cluster's name
    * @param version the version of the Ambari Service stack, e.g. `"HDP-1.2.0"`
    * @return the future of the created cluster
    */
  def createCluster(name: String, version: String): Future[Cluster] =
    performRequest(baseUrl / "clusters" / name << s"""{"Clusters": {"version": "$version"}}""")
      .flatMap(_ => getCluster(name))

  /** Remove the specified cluster.
    *
    * @param name the cluster's name
    * @return the future of the cluster removal
    */
  def removeCluster(name: String): Future[Unit] =
    performRequest(baseUrl.DELETE / "clusters" / name).map(_ => ())

  private val bootstrapSequencer = new SequentialOperations

  def bootstrapMachines(hostnames: Set[String], sshKey: String): Future[Unit] = {
    val configuredBuilder = (baseUrl / "bootstrap").POST
      .setBody(compact(render(
        ("hosts" -> hostnames) ~
        ("sshKey" -> sshKey) ~
        ("verbose" -> true))))
      .addHeader("Content-Type", "application/json")
    def extractId(response: JValue) = response \ "status" match {
      case JString("OK") =>
        (response \ "requestId").extract[Int]
      case JString("ERROR") => throw RequestException(
        configuredBuilder.build,
        s"Bootstrap request returned an error: ${response \\ "log"}")
      case _ => throw ServiceError(s"Unexpected Ambari response for bootstrap request: $response")
    }
    bootstrapSequencer enqueue {
      for {
        response <- performRequest(configuredBuilder)
        _ <- createRequestHandler(baseUrl / "bootstrap" / extractId(response)).ensureFinished
      } yield ()
    }
  }

  def registeredHostnames: Future[Set[String]] = for {
    json <- performRequest(baseUrl / "hosts" <<? Seq("fields" -> "Hosts/*"))
  } yield for {
    item <- (json \\ "items").children.toSet
    if (item \\ "host_status").extract[String] == "HEALTHY"
  } yield (item \\ "host_name").extract[String]
}
