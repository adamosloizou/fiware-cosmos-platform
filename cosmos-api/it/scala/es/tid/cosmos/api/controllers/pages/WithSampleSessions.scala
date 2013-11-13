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

import scala.Some
import scala.concurrent.Future

import play.api.mvc.{SimpleResult, Session}
import play.api.test.Helpers._
import play.api.test.FakeRequest
import play.api.libs.json.JsValue

import es.tid.cosmos.api.auth.oauth2.OAuthUserProfile
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.mocks.WithTestApplication
import es.tid.cosmos.api.profile.{UserState, Registration, UserId}

trait WithSampleSessions extends WithTestApplication {

  trait UserSession {
    val session: Session

    def request(path: String, method: String = GET) =
      FakeRequest(method, path).withSession(session.data.toSeq: _*)

    def doRequest(path: String, method: String = GET): Future[SimpleResult] =
      route(request(path, method)).get

    def submitForm(path: String, fields: (String, String)*): Future[SimpleResult] =
      route(request(path, POST).withFormUrlEncodedBody(fields: _*)).get

    def submitJson(path: String, body: JsValue, method: String = POST): Future[SimpleResult] =
      route(request(path, method).withJsonBody(body)).get
  }

  val unauthUser = new UserSession {
    val session = Session()
  }
  val unregUser = new UserSession {
    val userId = UserId("unreg")
    val userProfile = OAuthUserProfile(
      id = userId,
      name = Some("unregistered"),
      email = Some("unreg@mail.com")
    )
    val session = Session().setUserProfile(userProfile).setToken("token")
  }
  val regUser = new UserSession {
    val email = "user1@mail.com"
    val userProfile = OAuthUserProfile(
      id = UserId("user1"),
      name = Some("User 1"),
      email = Some(email)
    )
    val handle = "reguser"
    val cosmosProfile = dao.withTransaction { implicit c =>
      dao.registerUserInDatabase(userProfile.id,
        Registration(handle, s"ssh-rsa AAAAA $email", email))
    }
    val session = Session().setUserProfile(userProfile).setToken("token")
  }
  val disabledUser = new UserSession {
    val email = "disabled1@mail.com"
    val userProfile = OAuthUserProfile(
      id = UserId("disabled1"),
      name = Some("Disabled 1"),
      email = Some(email)
    )
    val handle = "disabled"
    val cosmosProfile = dao.withTransaction { implicit c =>
      val profile = dao.registerUserInDatabase(userProfile.id,
        Registration(handle, s"ssh-rsa AAAAA $email", email))
      dao.setUserState(profile.id, UserState.Disabled)
      profile
    }
    val session = Session().setUserProfile(userProfile).setToken("token")
  }
}
