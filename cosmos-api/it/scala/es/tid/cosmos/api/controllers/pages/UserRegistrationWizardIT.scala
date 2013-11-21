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

package es.tid.cosmos.api.controllers.pages

import org.mockito.ArgumentCaptor
import org.mockito.Matchers.any
import org.mockito.Mockito.{verify, spy}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.api.controllers.CosmosProfileTestHelpers._
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManager
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.wizards.UserRegistrationWizard
import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.clusters.ClusterId

class UserRegistrationWizardIT extends FlatSpec with MustMatchers with MockitoSugar {

  val handle = "jsmith"
  val userId = userIdFor(handle)
  val registration = registrationFor(handle)

  trait WithUserRegistrationWizard {
    val dao = new MockCosmosProfileDao()
    val sm = spy(new MockedServiceManager(transitionDelay = 0))
    val instance = new UserRegistrationWizard(dao, sm)
  }

  "User registration" must "create a new profile with the input data" in
    new WithUserRegistrationWizard {
      val profile = instance.registerUser(userId, registration)
      profile.handle must be (handle)
      profile.email must be (registration.email)
      profile.state must be (UserState.Enabled)
      profile.keys must have size 1
    }

  it must "reconfigure persistent HDFS cluster with current and deleted users" in
    new WithUserRegistrationWizard {
      val deletedUser = registerUser(dao, "deleted")
      dao.withTransaction { implicit c =>
        dao.setUserState(deletedUser.id, UserState.Deleted)
      }
      
      instance.registerUser(userId, registration)

      val usersCaptor = ArgumentCaptor.forClass(classOf[Seq[ClusterUser]])
      verify(sm).setUsers(any[ClusterId], usersCaptor.capture())
      val users = usersCaptor.getValue
      users.map(_.userName).toSet must be (Set("deleted", handle))
      users.find(_.userName == "deleted").get must (not be 'hdfsEnabled and not be 'sshEnabled)
      users.find(_.userName == handle).get must (be ('hdfsEnabled) and not be 'sshEnabled)
    }
}
