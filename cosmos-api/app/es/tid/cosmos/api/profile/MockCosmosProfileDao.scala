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

package es.tid.cosmos.api.profile

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.profile.UserState._

trait MockCosmosProfileDaoComponent extends CosmosProfileDaoComponent {
  def cosmosProfileDao: CosmosProfileDao = new MockCosmosProfileDao
}

/**
 * Mock to be used in tests for handling the profile DAO. Not thread-safe.
 */
class MockCosmosProfileDao extends CosmosProfileDao {

  object DummyConnection
  type Conn = DummyConnection.type

  private var users = Map[UserId, CosmosProfile]()
  private var clusters = List[ClusterAssignment]()

  def withConnection[A](block: (Conn) => A): A = block(DummyConnection)
  def withTransaction[A](block: (Conn) => A): A = block(DummyConnection)

  override def registerUser(userId: UserId, reg: Registration)(implicit c: Conn): CosmosProfile = {
    val credentials = ApiCredentials.random()
    require(!users.values.exists(_.handle == reg.handle), "Duplicated handle")
    val cosmosProfile = CosmosProfile(
      id = users.size,
      state = Enabled,
      handle = reg.handle,
      email = reg.email,
      quota = UnlimitedQuota,
      apiCredentials = credentials,
      keys = List(NamedKey("default", reg.publicKey))
    )
    users = users.updated(userId, cosmosProfile)
    cosmosProfile
  }

  override def getAllUsers()(implicit c: Conn): Seq[CosmosProfile] = users.values.toSeq

  override def getProfileId(userId: UserId)(implicit c: Conn): Option[ProfileId] =
    users.get(userId).map(_.id)

  override def getMachineQuota(id: ProfileId)(implicit c: Conn): Quota =
    users.collectFirst {
      case (_, profile) if profile.id == id => profile.quota
    }.getOrElse(EmptyQuota)

  override def setMachineQuota(id: ProfileId, quota: Quota)
                              (implicit c: Conn): Boolean = synchronized {
    users.collectFirst {
      case (userId, profile) if profile.id == id => {
        users = users.updated(userId, profile.copy(quota = quota))
        true
    }}.getOrElse(false)
  }

  override def handleExists(handle: String)(implicit c: Conn): Boolean =
    users.values.exists(_.handle == handle)

  override def lookupByUserId(userId: UserId)(implicit c: Conn): Option[CosmosProfile] =
    users.get(userId)

  override def lookupByApiCredentials(creds: ApiCredentials)
                                     (implicit c: Conn): Option[CosmosProfile] =
    users.collectFirst {
      case (_, profile) if profile.apiCredentials == creds => profile
    }

  override def assignCluster(assignment: ClusterAssignment)(implicit c: Conn) {
    synchronized {
      require(!clusters.exists(_.clusterId == assignment.clusterId), "Cluster already assigned")
      clusters = clusters :+ assignment
    }
  }

  override def clustersOf(id: ProfileId)(implicit c: Conn): Seq[ClusterAssignment] =
    clusters.filter(_.ownerId == id)

  override def setHandle(id: ProfileId, handle: String)(implicit c: Conn) {
    updateProfile(id) { profile =>
      require(
        profile.handle == handle || users.values.find(_.handle == handle).isEmpty,
        "duplicated handle"
      )
      profile.copy(handle = handle)
    }
  }

  override def setEmail(id: ProfileId, email: String)(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(email = email)
    }
  }

  override def setUserState(id: ProfileId, userState: UserState)(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(state = userState)
    }
  }

  override def setPublicKeys(id: ProfileId, publicKeys: Seq[NamedKey])(implicit c: Conn) {
    updateProfile(id) { profile =>
      profile.copy(keys = publicKeys)
    }
  }

  private def updateProfile(id: ProfileId)(f: CosmosProfile => CosmosProfile) {
    val maybeId = users.collectFirst {
      case (userId, profile) if profile.id == id => userId
    }
    maybeId.map(userId =>
      users = users.updated(userId, f(users(userId)))
    ).getOrElse(throw new IllegalArgumentException(s"No user with id=$id"))
  }
}
