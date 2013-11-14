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

import com.ning.http.client.{RequestBuilder, Request}
import net.liftweb.json.JsonAST.JNothing
import net.liftweb.json.JsonDSL._
import org.mockito.Matchers.any
import org.mockito.Mockito.{when, verify}
import org.scalatest.BeforeAndAfter
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.{RequestException, ServiceException}

class AmbariServerTest extends AmbariTestBase with BeforeAndAfter with MockitoSugar {
  var ambariServer: AmbariServer with MockedRestResponsesComponent = _

  before {
    ambariServer = new AmbariServer(
      serverUrl = "",
      port = 0,
      username = "",
      password = "")
      with FakeAmbariRestReplies with MockedRestResponsesComponent with RequestHandlerFactory {
      override def createRequestHandler(url: RequestBuilder): RequestHandler =
        new RequestHandler {
          def ensureFinished: Future[Unit] = Future.successful()
        }
    }
  }

  it must "fail when server authentication fails" in {
    when(ambariServer.responses.authorize(any[Request])).thenReturn(Some(
      Future.failed(new ServiceException("Invalid password"))))
    evaluating {
      get(ambariServer.listClusterNames)
    } must produce [ServiceException]
  }

  it must "return a list of cluster names" in {
    addMock(
      ambariServer.responses.listClusters,
      ("href" -> "http://some-bad-url") ~
      ("items" -> List("test1", "test2", "test3").map(name =>
        ("href" -> "http://another-bad-url") ~
        ("Clusters" -> (
          ("cluster_name" -> name) ~
          ("version" -> "banana")
        )))))
    val clusterNames = get(ambariServer.listClusterNames)
    clusterNames must have size (3)
    clusterNames(0) must be ("test1")
    clusterNames(1) must be ("test2")
    clusterNames(2) must be ("test3")
  }

  it must "return a cluster given a cluster name" in {
    addMock(
      ambariServer.responses.getCluster("test1"),
      ("href" -> "http://cluster-url") ~
      ("Clusters" -> ("cluster_name" -> "test1")))
    get(ambariServer.getCluster("test1")).name must be ("test1")
    get(ambariServer.getCluster("test1")).name must be ("test1")
  }

  it must "propagate failures when getting a cluster" in errorPropagation(
    ambariServer.responses.getCluster("badCluster"),
    ambariServer.getCluster("badCluster")
  )

  it must "be able to create a cluster" in {
    addMock(
      ambariServer.responses.getCluster("foo"),
      ("href" -> "http://cluster-url") ~
      ("Clusters" -> ("cluster_name" -> "foo")))
    addMock(
      ambariServer.responses.createCluster("foo"),
      JNothing)
    get(ambariServer.createCluster("foo", "bar")).name must be ("foo")
    verify(ambariServer.responses).createCluster("foo")
  }

  it must "propagate failures when creating a cluster" in errorPropagation(
    ambariServer.responses.createCluster("test1"),
    ambariServer.createCluster("test1", "bar")
  )

  it must "be able to remove a cluster" in {
    addMock(
      ambariServer.responses.removeCluster("foo"),
      JNothing)
    get(ambariServer.removeCluster("foo"))
    verify(ambariServer.responses).removeCluster("foo")
  }

  it must "propagate failures when removing" in errorPropagation(
    ambariServer.responses.removeCluster("badcluster"),
    ambariServer.removeCluster("badcluster")
  )

  it must "be able to bootstrap a set of hostnames" in {
    addMock(
      ambariServer.responses.bootstrap(any()),
      ("requestId" -> 1) ~
      ("status" -> "OK")
    )
    get(ambariServer.bootstrapMachines(
      Set("foo.com"),
      "dummy-ssh-key"))
    verify(ambariServer.responses).bootstrap(any())
  }

  it must "raise an error on failed bootstrap" in {
    addMock(
      ambariServer.responses.bootstrap(any()),
      ("status" -> "ERROR")
    )
    evaluating {
      get(ambariServer.bootstrapMachines(
        Set("foo.com"),
        "dummy-ssh-key"))
    } must produce [RequestException]
    verify(ambariServer.responses).bootstrap(any())
  }

  it must "propagate errors when bootstrapping" in errorPropagation(
    ambariServer.responses.bootstrap(any()),
    ambariServer.bootstrapMachines(
      Set("foo.com"),
      "dummy-ssh-key")
  )

  it must "be able to teardown a set of hostnames" in {
    addMock(
      ambariServer.responses.teardown(any()),
      ("requestId" -> 1) ~
      ("status" -> "OK")
    )
    get(ambariServer.teardownMachines(
      Set("foo.com"),
      "dummy-ssh-key"))
    verify(ambariServer.responses).teardown(any())
  }

  it must "propagate errors when teardown" in errorPropagation(
    ambariServer.responses.teardown(any()),
    ambariServer.teardownMachines(
      Set("foo.com"),
      "dummy-ssh-key")
  )

  it must "raise an error on failed teardown" in {
    addMock(
      ambariServer.responses.teardown(any()),
      ("status" -> "ERROR")
    )
    evaluating {
      get(ambariServer.teardownMachines(
        Set("foo.com"),
        "dummy-ssh-key"))
    } must produce [RequestException]
    verify(ambariServer.responses).teardown(any())
  }

  it must "provide the list of registered hostnames (multiple hosts)" in {
    addMock(
      ambariServer.responses.serverHosts,
      ("href" -> "www.some.server.com/api/v1/hosts") ~
      ("items" -> List("Hosts" -> ("host_name" -> "foo"), "Hosts" -> ("host_name" -> "bar")))
    )
    get(ambariServer.registeredHostnames) must be (Set("foo", "bar"))
    verify(ambariServer.responses).serverHosts
  }

  it must "provide the list of registered hostnames (single host)" in {
    addMock(
      ambariServer.responses.serverHosts,
      ("href" -> "www.some.server.com/api/v1/hosts") ~
      ("items" -> List("Hosts" -> ("host_name" -> "foo")))
    )
    get(ambariServer.registeredHostnames) must be (Set("foo"))
    verify(ambariServer.responses).serverHosts
  }
}
