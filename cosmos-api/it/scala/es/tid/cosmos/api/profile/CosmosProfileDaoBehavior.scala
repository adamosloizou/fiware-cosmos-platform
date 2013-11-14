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

import org.scalatest.{FlatSpec, Tag}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.UserState._
import es.tid.cosmos.servicemanager.clusters.ClusterId

trait CosmosProfileDaoBehavior { this: FlatSpec with MustMatchers =>

  type DaoTest = CosmosProfileDao => Unit

  def registration(handle: String) = Registration(
    handle = handle,
    publicKey = s"ssh-rsa pk00001 $handle@host",
    email = s"$handle@example.com"
  ) //TODO: Use this where possible

  def register(dao: CosmosProfileDao, id: UserId, reg: Registration)(implicit c: dao.Conn) =
    dao.registerUserInDatabase(id, reg, NoGroup, UnlimitedQuota)(c)

  def profileDao(withDao: DaoTest => Unit, maybeTag: Option[Tag] = None) {

    val unknownCosmosId = 1000

    def taggedTest(subject: ItVerbString, testFun: => Unit) =
      maybeTag.map(tag => subject taggedAs tag in testFun)
        .getOrElse(subject in testFun)

    taggedTest(it must "register new users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val id = UserId("db-0003")
        val profile = register(dao, id, registration("jsmith"))
        profile.handle must be ("jsmith")
        profile.keys.length must be (1)
        profile.email must be ("jsmith@example.com")
      }
    })

    taggedTest(it must "change the handle of users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val userId = UserId("db-0003")
        val cosmosId = register(dao, userId, registration("jsmith")).id
        dao.setHandle(cosmosId, "jsm")
        dao.lookupByUserId(userId).get.handle must equal ("jsm")
      }
    })

    taggedTest(it must "not change the handle of unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        evaluating {
          dao.setHandle(unknownCosmosId, "jsm")
        } must produce [IllegalArgumentException]
      }
    })

    taggedTest(it must "change the state of users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val userId = UserId("db-0003")
        val cosmosId = register(dao, userId, registration("jsmith")).id
        dao.setUserState(cosmosId, Deleting)
        dao.lookupByUserId(userId).get.state must equal (Deleting)
      }
    })

    taggedTest(it must "not change the state of unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        evaluating {
          dao.setUserState(unknownCosmosId, Disabled)
        } must produce [IllegalArgumentException]
      }
    })

    taggedTest(it must "not change the handle to a repeated one", withDao { dao =>
      dao.withTransaction { implicit c =>
        register(dao, UserId("db001"), registration("existing"))
        val cosmosId = register(dao, UserId("db002"), registration("current")).id
        evaluating {
          dao.setHandle(cosmosId, "existing")
        } must produce [IllegalArgumentException]
      }
    })

    taggedTest(it must "change the keys of users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val userId = UserId("db-0003")
        val cosmosId = register(dao, userId, registration("jsmith")).id
        val newKeys = Seq(
          NamedKey("default", "ssh-rsa AAAAA jsmith@host"),
          NamedKey("extra", "ssh-rsa BBBBB jsmith@host"))
        dao.setPublicKeys(cosmosId, newKeys)
        dao.lookupByUserId(userId).get.keys must equal (newKeys)
      }
    })

    taggedTest(it must "not change the keys of unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        evaluating {
          dao.setPublicKeys(unknownCosmosId, Seq(NamedKey("default", "ssh-rsa AAAAA jsmith@host")))
        } must produce [IllegalArgumentException]
      }
    })

    taggedTest(it must "detect unused handles", withDao { dao =>
      dao.withTransaction { implicit c =>
        register(dao, UserId("oauth53"), registration("usedHandle"))
        dao.handleExists("usedHandle") must be (true)
        dao.handleExists("unusedHandle") must be (false)
      }
    })

    taggedTest(it must "get Cosmos ID from user ID when user is registered", withDao { dao =>
      dao.withTransaction { implicit c =>
        val registeredUser = UserId("db-registered")
        register(dao, registeredUser, registration("jsmith"))
        dao.getCosmosId(registeredUser) must be ('defined)
        dao.getCosmosId(UserId("db-unknown")) must not be 'defined
      }
    })

    taggedTest(it must "get empty machine quota for unknown users", withDao { dao =>
      dao.withTransaction { implicit c =>
        val unknownUserId : Long = 1001
        dao.getMachineQuota(unknownUserId) must equal (EmptyQuota)
      }
    })

    taggedTest(it must "lookup a profile from api credentials", withDao { dao =>
      dao.withTransaction { implicit c =>
        val id = UserId("db-0004")
        register(dao, id, registration("user4"))
        val profileByUserId = dao.lookupByUserId(id).get
        val profileByApiCredentials =
          dao.lookupByApiCredentials(profileByUserId.apiCredentials).get
        profileByUserId must be (profileByApiCredentials)
      }
    })

    taggedTest(it must "assign cluster ownership and remember it", withDao { dao =>
      dao.withTransaction { implicit c =>
        val clusterId = ClusterId()
        val id1 = register(dao, UserId("user1"), registration("user1")).id
        val id2 = register(dao, UserId("user2"), registration("user2")).id
        dao.assignCluster(clusterId, id2)
        dao.clustersOf(id1).map(_.clusterId).toList must not contain clusterId
        dao.clustersOf(id2).map(_.clusterId).toList must contain (clusterId)
      }
    })

    taggedTest(it must "get empty, NoGroup by default", withDao{ dao =>
      dao.withConnection{ implicit c =>
        dao.getGroups must equal (Set(NoGroup))
      }
    })

    taggedTest(it must "get all registered groups + NoGroup", withDao{ dao =>
      val (a, b, c) = (
        GuaranteedGroup("A", EmptyQuota),
        GuaranteedGroup("B", Quota(2)),
        GuaranteedGroup("C", Quota(3))
      )
      dao.withTransaction{ implicit conn =>
        Seq(a, b, c).foreach(dao.registerGroup)
        dao.getGroups must be (Set(a, b, c, NoGroup))
      }
    })

    taggedTest(it must "lookup users by group", withDao{ dao =>
      dao.withTransaction{ implicit c =>
        val group = GuaranteedGroup("A", Quota(3))
        dao.registerGroup(group)
        val id1 = dao.registerUserInDatabase(
          UserId("user1"), registration("user1"), NoGroup, UnlimitedQuota).id
        val id2 = dao.registerUserInDatabase(
          UserId("user2"), registration("user2"), group, UnlimitedQuota).id
        val noGroupProfiles = dao.lookupByGroup(NoGroup)
        val groupProfiles = dao.lookupByGroup(group)
        noGroupProfiles must have size 1
        noGroupProfiles.head.id must equal(id1)
        groupProfiles must have size 1
        groupProfiles.head.id must equal(id2)
      }
    })
  }
}
