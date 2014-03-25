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

package es.tid.cosmos.api.controllers.storage

import scalaz._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.controllers.common.Message
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager.ClusterProperties
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.quota.{Quota, GuaranteedGroup}
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters._

class InfinityAuthenticatorTest extends FlatSpec with MustMatchers {

  import Scalaz._

  trait WithAuthentication extends MockCosmosDataStoreComponent {
    val serviceManager = new MockedServiceManager()
    val instance = new InfinityAuthenticator(store, serviceManager)
    val profile = CosmosProfileTestHelpers.registerUser("user")(store)
    val clusterId = ClusterId.random()

    def createCluster(targetState: ClusterState = Running): ImmutableClusterDescription = {
      val cluster = serviceManager.defineCluster(ClusterProperties(
        id = clusterId,
        name = ClusterName("my private cluster"),
        size = 2,
        users = Set.empty
      ))
      cluster.setState(targetState)
      cluster.view
    }

    def registerCluster(shared: Boolean = false) = store.withTransaction { implicit c =>
      store.cluster.register(clusterId, profile.id, ClusterSecret.random(), shared)
    }.secret.get
  }

  "API key and secret authentication" must "be accepted with 777 mask and for any host" in
    new WithAuthentication {
      instance.authenticateApiCredentials(profile.apiCredentials) must be (InfinityIdentity(
        user = "user",
        group = "No Group",
        accessMask = AccessMask("777"),
        origins = AnyHost
      ).success)
    }

  it must "be rejected if credentials are unknown" in new WithAuthentication {
    val unknownCredentials = ApiCredentials.random()
    instance.authenticateApiCredentials(unknownCredentials) must
      be (Message("Unknown API credentials").failure)
  }

  it must "be rejected if the user profile is not enabled" in new WithAuthentication {
    store.withTransaction { implicit c =>
      store.profile.setUserState(profile.id, UserState.Disabled)
    }
    instance.authenticateApiCredentials(profile.apiCredentials) must
      be (Message("User is not enabled").failure)
  }

  it must "reflect the current group of the user" in new WithAuthentication {
    val group = GuaranteedGroup("myGroup", Quota(5))
    store.withTransaction { implicit c =>
      store.group.register(group)
      store.profile.setGroup(profile.id, group)
    }
    instance.authenticateApiCredentials(profile.apiCredentials).map(_.group) must
      be ("myGroup".success)
  }

  "Cluster secret authentication" must "be accepted with 777 mask and just for the cluster hosts" in
    new WithAuthentication {
      val cluster = createCluster()
      val clusterSecret = registerCluster()
      val result = instance.authenticateClusterSecret(clusterSecret)
      result.map(_.accessMask) must be (AccessMask("777").success)
      result.map(_.origins) must be (WhiteList(cluster.hosts.map(_.ipAddress)).success)
    }

  it must "be rejected if cluster is defined but not yet exists" in new WithAuthentication {
    val clusterSecret = registerCluster()
    instance.authenticateClusterSecret(clusterSecret) must
      be (Message("Cluster does not yet exist").failure)
  }

  it must "be rejected if cluster is not in running state" in new WithAuthentication {
    createCluster(targetState = Terminated)
    val clusterSecret = registerCluster()
    instance.authenticateClusterSecret(clusterSecret) must
      be (Message("Cluster is not in running state").failure)
  }

  it must "be accepted with 077 mask for shared clusters" in new WithAuthentication {
    val cluster = createCluster()
    val clusterSecret = registerCluster(shared = true)
    instance.authenticateClusterSecret(clusterSecret).map(_.accessMask) must
      be (AccessMask("077").success)
  }
}
