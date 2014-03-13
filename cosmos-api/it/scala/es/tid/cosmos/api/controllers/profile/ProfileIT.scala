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

package es.tid.cosmos.api.controllers.profile

import scala.language.reflectiveCalls

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.AuthBehaviors
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import play.api.libs.json.Json

class ProfileIT extends FlatSpec with MustMatchers with AuthBehaviors {

  val profileResource = "/cosmos/v1/profile"
  val templateProfile = Json.obj(
    "handle" -> "handle",
    "email" -> "jsmith@example.com",
    "keys" -> Json.arr(Json.obj(
      "name" -> "default",
      "signature" -> "ssh-rsa DKDJDJDK jsmith@example.com"
    ))
  )

  "The profile resource" must behave like
    rejectingUnauthenticatedRequests(FakeRequest(GET, profileResource))

  it must "return profile information about the credentials' owner" in new WithSampleSessions {
    val res = regUserInGroup.doRequest(profileResource)
    status(res) must be (OK)
    contentAsString(res) must include (regUserInGroup.handle)
  }

  "Updates to the profile" must behave like rejectingUnauthenticatedRequests(
    FakeRequest(PUT, profileResource).withJsonBody(templateProfile))

  it must "accept public key changes" in new WithSampleSessions {
    val newProfile = templateProfile ++ Json.obj("handle" -> regUserInGroup.handle)
    status(regUserInGroup.submitJson(profileResource, newProfile, method = PUT)) must be (OK)
  }

  it must "reject profile updates with other than one public key" in new WithSampleSessions {
    val newProfile = templateProfile ++ Json.obj(
      "handle" -> regUserInGroup.handle,
      "keys" -> Json.arr()
    )
    val response = regUserInGroup.submitJson(profileResource, newProfile, method = PUT)
    status(response) must be (BAD_REQUEST)
    contentAsString(response) must include ("Only one public key is supported")
  }

  it must "reject profile updates with a different handle" in new WithSampleSessions {
    val newProfile = templateProfile ++ Json.obj("handle" -> "newHandle")
    val response = regUserInGroup.submitJson(profileResource, newProfile, method = PUT)
    status(response) must be (BAD_REQUEST)
    contentAsString(response) must include ("Handle modification is not supported")
  }
}
