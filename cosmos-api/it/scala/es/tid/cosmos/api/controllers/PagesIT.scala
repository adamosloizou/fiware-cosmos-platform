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

import java.net.URI

import org.apache.commons.lang3.StringEscapeUtils
import org.scalatest.FlatSpec
import org.scalatest.matchers.{MatchResult, Matcher, MustMatchers}
import play.api.Play.current
import play.api.db.DB
import play.api.mvc.{Session, Result}
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.pages.{CosmosSession, Registration}
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.oauth2.UserProfile
import es.tid.cosmos.api.mocks.{WithMockedIdentityService, User}
import es.tid.cosmos.api.profile.CosmosProfileDao

class PagesIT extends FlatSpec with MustMatchers {
  "A non registered user" must "be authenticated and get to the registration page" in
    new WithMockedIdentityService {
      val authUrl = mustLinkTheAuthorizationProvider()
      val redirectUrl = identityService.requestAuthorizationCode(
        authUrl, identityService.users.head.id)
      val redirection = route(FakeRequest(GET, relativePath(redirectUrl))).get
      redirection must redirectTo ("/")
      val cosmosSession: CosmosSession = session(redirection)
      cosmosSession must be ('authenticated)
      cosmosSession must not be ('registered)
    }

  "A registered user" must "be authenticated and redirected to its profile" in
    new WithMockedIdentityService {
      registerUsers(identityService.users)
      val authUrl = mustLinkTheAuthorizationProvider()
      val redirectUrl = identityService.requestAuthorizationCode(
        authUrl, identityService.users.head.id)
      val redirection = route(FakeRequest(GET, relativePath(redirectUrl))).get
      redirection must redirectTo ("/")
      val cosmosSession: CosmosSession = session(redirection)
      cosmosSession must be ('authenticated)
      cosmosSession must be ('registered)
    }

  "A user rejecting authorization" must "see error information" in new WithMockedIdentityService {
    val authUrl = mustLinkTheAuthorizationProvider()
    val redirectUrl = identityService.requestAuthorizationCode(
      authUrl, identityService.users.head.id, isAuthorized = false)
    val result = route(FakeRequest(GET, relativePath(redirectUrl))).get
    status(result) must be (UNAUTHORIZED)
    contentAsString(result) must include ("Access denied")
  }

  "A user with an invalid token" must "see error information and lose her session" in
    new WithMockedIdentityService {
      val result = route(FakeRequest(GET, "/auth?code=invalid")).get
      val cosmosSession: CosmosSession = session(result)
      cosmosSession must not be ('authenticated)
      cosmosSession must not be ('registered)
      status(result) must be (UNAUTHORIZED)
      contentAsString(result) must include ("Authorization failed")
    }

  "The / route" must "link to the authorization provider when unauthorized" in
    new WithMockedIdentityService {
      mustLinkTheAuthorizationProvider()
    }

  it must "show the registration page when authorized but not registered" in
    new WithMockedIdentityService {
      val session = new Session()
        .setUserProfile(UserProfile(identityService.users.head.id))
        .setToken("token")
      val registrationPage = route(FakeRequest(GET, "/").withSession(session.data.toSeq: _*)).get
      status(registrationPage) must equal (OK)
      contentAsString(registrationPage) must include ("User registration")
    }

  it must "show the user profile when authorized and registered" in new WithMockedIdentityService {
    registerUsers(identityService.users)
    val user = identityService.users.head
    val session = new Session()
      .setUserProfile(UserProfile(user.id))
      .setToken("token")
      .setCosmosId(1)
    val registrationPage = route(FakeRequest(GET, "/").withSession(session.data.toSeq: _*)).get
    status(registrationPage) must equal (OK)
    contentAsString(registrationPage) must include ("Cosmos user profile")
  }

  private def mustLinkTheAuthorizationProvider(): String = {
    val home = route(FakeRequest(GET, "/")).get
    status(home) must equal (OK)
    val authUrl = authenticationUrl(contentAsString(home))
    authUrl must be ('defined)
    authUrl.get
  }

  private def registerUsers(users: Iterable[User]) {
    DB.withConnection { implicit c =>
      for (User(tuId, _, _, email) <- users) {
        val handle = email.map(_.split('@')(0)).getOrElse("root")
        CosmosProfileDao.registerUserInDatabase(tuId, Registration(handle, "pk1234"))
      }
    }
  }

  private def authenticationUrl(page: String) =
    """<a id="login" href="(.*?)">""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1)))

  private def relativePath(url: String) = {
    val uri = URI.create(url)
    s"${uri.getPath}?${uri.getQuery}"
  }

  private def redirectTo(path: String): Matcher[Result] =
    new Matcher[Result] {
      def apply(r: Result): MatchResult =
        if (status(r) != SEE_OTHER) {
          MatchResult(
            matches = false,
            failureMessage = s"${status(r)} status found when $SEE_OTHER were expected",
            negatedFailureMessage = s"${status(r)} status not found when $SEE_OTHER were expected"
          )
        } else {
          MatchResult(
            matches = header("Location", r) == Some(path),
            failureMessage = s"result does not redirect to $path",
            negatedFailureMessage = s"result redirects to $path"
          )
        }
    }
}
