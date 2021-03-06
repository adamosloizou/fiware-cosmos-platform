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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.cluster.CreateClusterParams
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.mocks.SampleClusters
import es.tid.cosmos.api.profile.ClusterSecret
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters.ClusterId
import es.tid.cosmos.servicemanager.services.Hdfs

class ClustersIT
  extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors {

  val validCreationParams =
    Json.toJson(CreateClusterParams(ClusterName("cluster_new"), 6, Seq(Hdfs.name), shared = false))
  val inValidCreationParams = Json.obj("invalid" -> "json")
  val resourcePath = "/cosmos/v1/cluster"
  val listClusters = FakeRequest(GET, resourcePath)
  val createCluster = FakeRequest(POST, resourcePath).withJsonBody(validCreationParams)

  "Cluster listing" must behave like rejectingUnauthenticatedRequests(listClusters)

  it must behave like enabledWhenUnderMaintenance(listClusters)

  "Cluster creation" must behave like rejectingUnauthenticatedRequests(createCluster)

  it must behave like enabledOnlyForOperatorsWhenUnderMaintenance(createCluster)

  it must "create shared clusters by default" in new WithSampleSessions {
    val resource = regUserInGroup.doRequest(FakeRequest(POST, resourcePath)
      .withJsonBody(Json.obj("name" -> "cluster_new", "size" -> 6)))
    status(resource) must equal (CREATED)
    contentType(resource) must be (Some("application/json"))
  }

  "The clusters resource" must "list user clusters" in new WithSampleSessions with SampleClusters {
    val user1 = new RegisteredUserSession("user1", "User 1")
    val user2 = new RegisteredUserSession("user2", "User 2")
    val ownCluster = SampleClusters.RunningClusterProps.id
    val otherCluster = ClusterId.random()
    store.withTransaction { implicit c =>
      store.cluster.register(ownCluster, user1.cosmosProfile.id, ClusterSecret.random())
      store.cluster.register(otherCluster, user2.cosmosProfile.id, ClusterSecret.random())
    }

    val resource = user1.doRequest(listClusters)

    store.withTransaction { implicit c =>
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      contentAsString(resource) must include (ownCluster.toString)
      contentAsString(resource) must include (SampleClusters.RunningClusterProps.name.underlying)
      contentAsString(resource) must not include otherCluster.toString
    }
  }

  it must "start a new cluster if no services are specified" in new WithSampleSessions {
    val resource = regUserInGroup.doRequest(FakeRequest(POST, resourcePath)
      .withJsonBody(Json.obj("name" -> "cluster_new", "size" -> 6)))
    status(resource) must equal (CREATED)
    contentType(resource) must be (Some("application/json"))
    val location = header("Location", resource)
    location must be ('defined)
    contentAsString(resource) must include (location.get)
    store.withConnection { implicit c =>
      store.cluster.ownedBy(regUserInGroup.cosmosProfile.id) must have length 1
    }
  }

  it must "start a new cluster if some services are specified" in new WithSampleSessions {
    val resource = regUserInGroup.doRequest(createCluster)
    status(resource) must equal (CREATED)
    contentType(resource) must be (Some("application/json"))
    val location = header("Location", resource)
    location must be ('defined)
    contentAsString(resource) must include (location.get)
    store.withConnection { implicit c =>
      store.cluster.ownedBy(regUserInGroup.cosmosProfile.id) must have length 1
    }
  }

  it must "reject cluster creation with invalid payload" in new WithSampleSessions {
    val resource =
      regUserInGroup.doRequest(FakeRequest(POST, resourcePath).withJsonBody(inValidCreationParams))
    status(resource) must equal (BAD_REQUEST)
  }
}
