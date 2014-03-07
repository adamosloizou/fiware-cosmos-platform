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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.concurrent.Eventually
import org.scalatest.matchers.MustMatchers
import org.scalatest.time.{Millis, Seconds, Span}
import play.api.libs.json.{Json, JsString, JsNumber}
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager.ClusterProperties
import es.tid.cosmos.api.profile.CosmosProfile
import es.tid.cosmos.api.test.matchers.JsonMatchers
import es.tid.cosmos.api.quota.{Quota, GuaranteedGroup}
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.{ClusterUser, ClusterName}
import es.tid.cosmos.servicemanager.clusters._

class InfoIT extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors
  with JsonMatchers with Eventually with FutureMatchers {

  override implicit def patienceConfig =
    PatienceConfig(timeout = Span(5, Seconds), interval = Span(100, Millis))

  val getInfo = FakeRequest(GET, "/cosmos/v1/info")

  "A general info request" must behave like rejectingUnauthenticatedRequests(getInfo)

  it must behave like enabledWhenUnderMaintenance(getInfo)

  it must "provide profile information about id, handle and individual quota" in
    new WithSampleSessions {
      val res = regUser.doRequest(getInfo)
      status(res) must be (OK)
      val quota = regUser.cosmosProfile.quota.toOptInt.get
      contentAsJson(res) must (
        containFieldWithValue("profileId", JsNumber(regUser.cosmosProfile.id)) and
        containFieldWithValue("handle", JsString(regUser.handle)) and
        containFieldWithValue("individualQuota", JsNumber(quota))
      )
    }

  it must "provide group name and group quota" in new WithSampleSessions {
    dao.store.withTransaction { implicit c =>
      dao.group.register(GuaranteedGroup("fooGroup", Quota(10)))
      dao.profile.setGroup(regUser.cosmosProfile.id, Some("fooGroup"))
    }
    val res = regUser.doRequest(getInfo)
    status(res) must be (OK)
    contentAsJson(res) must containFieldThatMust("group",
      containFieldWithValue("name", JsString("fooGroup")) and
      containFieldWithValue("guaranteedQuota", JsNumber(10))
    )
  }

  it must "provide info about clusters owned or accessible by SSH" in
    new WithSampleSessions {
      val cluster1 = mockedServiceManager.createCluster(
        name = ClusterName("ownCluster"),
        size = 2,
        serviceDescriptions = Seq.empty,
        users = Seq.empty
      )
      regUser.setAsOwner(cluster1)
      val cluster2 = ClusterId("cluster2")
      mockedServiceManager.defineCluster(ClusterProperties(
        id = cluster2,
        name = ClusterName("cluster2"),
        users = Set(opUser.asClusterUser(), regUser.asClusterUser()),
        size  = 2,
        initialState = Some(Running)
      ))
      mockedServiceManager.createCluster(
        name = ClusterName("unlisted"),
        size = 2,
        serviceDescriptions = Seq.empty,
        users = Seq(opUser.asClusterUser(), regUser.asClusterUser(sshEnabled = false))
      )

      val res = regUser.doRequest(getInfo)
      status(res) must be (OK)

      contentAsJson(res) must containFieldThatMust("clusters",
        containFieldWithValue("owned", Json.arr(cluster1.toString)) and
        containFieldWithValue("accessible", Json.arr(cluster2.toString))
      )
    }

  it must "filter terminated clusters from the owned listing" in
    new WithSampleSessions {
      val cluster1 = mockedServiceManager.defineCluster(MockedServiceManager.ClusterProperties(
        id = ClusterId(),
        name = ClusterName("own but terminated"),
        size = 10,
        users = Set(regUser.asClusterUser()),
        initialState = Some(Terminated)
      ))
      regUser.setAsOwner(cluster1.view.id)

      val res = regUser.doRequest(getInfo)
      status(res) must be (OK)

      contentAsJson(res) must containFieldThatMust("clusters",
        containFieldWithValue("owned", Json.arr())
      )
    }

  it must "filter non-running clusters from the accessible listing" in
    new WithSampleSessions {
      val cluster1 = mockedServiceManager.defineCluster(ClusterProperties(
        id = ClusterId("cluster1"),
        name = ClusterName("added to but not running"),
        users = Set(opUser.asClusterUser(), regUser.asClusterUser()),
        size  = 2,
        initialState = Some(Provisioning)
      ))
      opUser.setAsOwner(cluster1.view.id)

      val res = regUser.doRequest(getInfo)
      status(res) must be (OK)

      contentAsJson(res) must containFieldThatMust("clusters",
        containFieldWithValue("accessible", Json.arr())
      )
    }

  it must "provide info about existing and available resources" in new WithSampleSessions {
    dao.store.withTransaction { implicit c =>
      dao.group.register(GuaranteedGroup("fooGroup", Quota(10)))
      dao.group.register(GuaranteedGroup("otherGroup", Quota(5)))
      dao.profile.setGroup(regUser.cosmosProfile.id, Some("fooGroup"))
    }
    val cluster = mockedServiceManager.createCluster(
      name = ClusterName("ownCluster"),
      size = 2,
      serviceDescriptions = Seq.empty,
      users = Seq.empty
    )
    regUser.setAsOwner(cluster)
    val res = regUser.doRequest(getInfo)
    status(res) must be (OK)
    val quota = regUser.cosmosProfile.quota.toOptInt.get
    contentAsJson(res) must containFieldThatMust("resources",
      containFieldWithValue("groupConsumption", JsNumber(2)) and
      containFieldWithValue("individualConsumption", JsNumber(2)) and
      containFieldWithValue("available",
        JsNumber(mockedServiceManager.clusterNodePoolCount - 2)) and
      containFieldWithValue("availableForGroup",
        JsNumber(mockedServiceManager.clusterNodePoolCount - 5 - 2)) and
      containFieldWithValue("availableForUser",
        JsNumber(CosmosProfile.DefaultQuota.toOptInt.get - 2))
    )
  }
}
