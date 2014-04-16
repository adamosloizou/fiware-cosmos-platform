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

import scala.concurrent.{Await, Future}
import scala.concurrent.duration.Duration

import com.ning.http.client.{Request, RequestBuilder}
import net.liftweb.json.JsonAST._
import net.liftweb.json.parse
import org.mockito.Matchers._
import org.mockito.Mockito._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import es.tid.cosmos.servicemanager.ServiceException

trait FakeAmbariRestReplies extends RequestProcessor {
  this: MockedRestResponsesComponent =>

  override def performRequest(requestBuilder: RequestBuilder): Future[JValue] = {
    val request= requestBuilder.build()
    val authorizationResponse = responses.authorize(request)
    if (!authorizationResponse.isEmpty) {
      return authorizationResponse.get
    }
    val GET = "GET"
    val POST = "POST"
    val DELETE = "DELETE"
    val PUT = "PUT"
    val AllClusters = ".+/api/v1/clusters".r
    val SpecificCluster = ".+/api/v1/clusters/([^/]+)".r
    val AllServices = ".+/api/v1/.*services".r
    val SpecificService = ".+/api/v1/.*services/([^/]+)".r
    val SpecificComponent = ".+/api/v1/.+/components/([^/]+)".r
    val AllHosts = ".+/api/v1/clusters/[^/]+/hosts".r
    val SpecificHostQuery = """.+/api/v1/.*hosts\?Hosts%2Fhost_name=(.+)""".r
    val AllServerHosts = """.+/api/v1/hosts\?fields=Hosts%2F%2A""".r
    val SpecificHost = ".+/api/v1/.*hosts/([^/]+)".r
    val SpecificRequest = """.+/api/v1/.*request/([^/]+)\?.+""".r
    val BootStrap = """.+/api/v1/bootstrap""".r
    val BootStrapRequest = """.+/api/v1/bootstrap/([^/]+)""".r
    (request.getMethod, request.getUrl) match {
      case (GET, AllClusters()) => responses.listClusters
      case (GET, SpecificCluster(name)) => responses.getCluster(name)
      case (POST, SpecificCluster(name)) => responses.createCluster(name)
      case (DELETE, SpecificCluster(name)) => responses.removeCluster(name)
      case (PUT, SpecificCluster(name)) =>
        responses.applyConfiguration(name, parse(request.getStringData) \\ "properties")
      case (GET, SpecificService(name)) => responses.getService(name)
      case (GET, AllServerHosts()) => responses.serverHosts
      case (POST, AllServices()) => responses.addService(request.getStringData)
      case (GET, SpecificHost(name)) => responses.getHost(name)
      case (POST, AllHosts()) => responses.addHost(request.getStringData)
      case (POST, SpecificHostQuery(name)) =>
        responses.addHostComponent(name, request.getStringData)
      case (POST, SpecificComponent(name)) => responses.addServiceComponent(name)
      case (PUT, SpecificService(name)) => responses.changeServiceState(name, request.getStringData)
      case (GET, SpecificRequest(name)) => responses.getRequest(name)
      case (POST, BootStrap()) => responses.bootstrap(request.getStringData)
      case (DELETE, BootStrap()) => responses.teardown(request.getStringData)
      case (GET, BootStrapRequest(id)) => responses.bootstrapRequest(id)
    }
  }
}

trait MockedRestResponsesComponent extends MockitoSugar {
  val responses = mock[RestResponses]
  when(responses.authorize(any[Request])).thenReturn(None)

  trait RestResponses {
    def authorize(request: Request): Option[Future[JValue]]
    def listClusters: Future[JValue]
    def getCluster(name: String): Future[JValue]
    def createCluster(name: String): Future[JValue]
    def removeCluster(name: String): Future[JValue]
    def getService(name: String): Future[JValue]
    def addService(body: String): Future[JValue]
    def getHost(name: String): Future[JValue]
    def addHost(body: String): Future[JValue]
    def applyConfiguration(name: String, properties: JValue): Future[JValue]
    def addHostComponent(name: String, body: String): Future[JValue]
    def addServiceComponent(name: String): Future[JValue]
    def changeServiceState(name: String, body: String): Future[JValue]
    def getRequest(name: String): Future[JValue]
    def bootstrap(body: String): Future[JValue]
    def teardown(body: String): Future[JValue]
    def bootstrapRequest(id: String): Future[JValue]
    def serverHosts: Future[JValue]
  }
}

trait AmbariTestBase extends FlatSpec with MustMatchers {
  def addMock(mockCall: => Future[JValue], success: JValue, subsequentValues: JValue*) {
    when(mockCall).thenReturn(
      Future.successful(success),
      subsequentValues.map(Future.successful(_)): _*)
  }

  def get[T](future: Future[T]) = Await.result(future, Duration.Inf)

  def errorPropagation(mockCall: => Future[JValue], call: => Future[Any]) {
    when(mockCall).thenReturn(Future.failed(new ServiceException("Error")))
    evaluating {
      get(call)
    } must produce [ServiceException]
  }
}
