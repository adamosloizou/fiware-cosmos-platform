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

package es.tid.cosmos.api.controllers.common

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.db.DB
import play.api.mvc._
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.mocks.WithInMemoryDatabase
import es.tid.cosmos.api.authorization.ApiCredentials
import es.tid.cosmos.api.profile.CosmosProfileDao
import es.tid.cosmos.api.controllers.pages.Registration
import es.tid.cosmos.api.controllers.pages.CosmosProfile

class AuthControllerIT extends FlatSpec with MustMatchers {

  object TestController extends AuthController {
    def index() = Action(parse.anyContent) { request =>
      Authenticated(request) { profile =>
        Ok(s"handle=${profile.handle}")
      }
    }
  }

  val action: Action[AnyContent] = TestController.index()
  val request: Request[AnyContent] = FakeRequest(GET, "/some/path")

  "Auth controller" must "not authorize when authorization header is missing" in
    new WithInMemoryDatabase {
      val response = action(request)
      status(response) must be (UNAUTHORIZED)
      contentAsString(response) must include ("Missing authorization header")
    }

  it must "not authorize when authorization header is malformed" in new WithInMemoryDatabase {
    val response = action(authorizedRequest("Basic malformed"))
    status(response) must be (UNAUTHORIZED)
    contentAsString(response) must include ("Malformed authorization header")
  }

  it must "return bad request when credentials are invalid" in new WithInMemoryDatabase {
    val response = action(authorizedRequest(ApiCredentials.random()))
    status(response) must be (UNAUTHORIZED)
    contentAsString(response) must include ("Invalid API credentials")
  }

  it must "succeed when credentials are valid" in new WithInMemoryDatabase {
    DB.withConnection { implicit c =>
      CosmosProfileDao.registerUserInDatabase("db000", Registration("login", "pk"))
      val profile: CosmosProfile = CosmosProfileDao.lookupByUserId("db000").get
      val result: Result = action(authorizedRequest(profile.apiCredentials))
      status(result) must be (OK)
      contentAsString(result) must include ("handle=login")
    }
  }

  private def authorizedRequest(credentials: ApiCredentials): Request[AnyContent] =
    authorizedRequest(BasicAuth(credentials.apiKey, credentials.apiSecret))

  private def authorizedRequest(authorizationHeader: String): Request[AnyContent] = FakeRequest(
    method = GET,
    uri = "/some/path",
    headers = FakeHeaders(Seq(
      "Authorization" -> Seq(authorizationHeader))),
    body = AnyContentAsEmpty
  )
}
