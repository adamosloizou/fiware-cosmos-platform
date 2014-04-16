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

import scala.language.reflectiveCalls

import org.apache.commons.lang3.StringEscapeUtils
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.controllers.ResultMatchers.redirectTo
import es.tid.cosmos.api.controllers.pages.{WithSampleSessions, CosmosSession}
import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.mocks.{MockAuthConstants, WithTestApplication}

class PagesIT extends FlatSpec with MustMatchers with AuthBehaviors with MaintenanceModeBehaviors {

  "The index page" must "show the landing page with auth links for unauthorized users" in
    new WithSampleSessions {
      val landingPage = unauthUser.doRequest("/")
      status(landingPage) must equal (OK)
      contentAsString(landingPage) must include ("id=\"auth-selector\"")
      authenticationUrl(contentAsString(landingPage)) must be ('defined)
    }

  it must behave like enabledWhenUnderMaintenance(FakeRequest(GET, "/"))

  it must "redirect to the registration form for the unregistered users" in new WithSampleSessions {
    unregUser.doRequest("/") must redirectTo ("/register")
  }

  it must "redirect to the user profile page for registered users" in new WithSampleSessions {
    regUserInGroup.doRequest("/") must redirectTo ("/profile")
  }

  "The registration page" must "redirect to the index unauthenticated users" in
    new WithSampleSessions {
      unauthUser.doRequest("/register") must redirectTo ("/")
    }

  it must behave like pageDisabledWhenUnderMaintenance(FakeRequest(GET, "/register"))
  it must behave like resourceDisabledWhenUnderMaintenance(FakeRequest(POST, "/register"))

  it must "show the registration form to the unregistered users" in new WithSampleSessions {
    val registrationPage = unregUser.doRequest("/register")
    status(registrationPage) must equal (OK)
    contentAsString(registrationPage) must include ("User registration")
  }

  it must "log out users after cancelling the registration process" in new WithSampleSessions {
    val registrationPage = unregUser.doRequest("/register")
    val url = cancelRegistrationUrl(contentAsString(registrationPage))
    val response = unregUser.doRequest(url)
    response must redirectTo ("/")
    val responseSession: CosmosSession = session(response)
    responseSession must not be 'authenticated
  }

  it must "redirect to the index registered users" in new WithSampleSessions {
    regUserInGroup.doRequest("/register") must redirectTo ("/")
  }

  it must "register unregistered users" in new WithSampleSessions {
    withPersistentHdfsDeployed {
      val response = unregUser.submitForm("/register",
        "handle" -> "newuser",
        "email" -> "jsmith@example.com",
        "publicKey" -> "ssh-rsa DKDJDJDK jsmith@example.com")
      response must redirectTo ("/")
      store.withConnection { implicit c =>
        store.profile.lookupByUserId(unregUser.userId) must be ('defined)
      }
    }
  }

  it must "reject registrations when the submitted form is invalid" in new WithSampleSessions {
    val response = unregUser.submitForm("/register",
      "handle" -> "1nvalid handle",
      "email" -> "not an email",
      "publicKey" -> "ssh-rsa DKDJDJDK ;;;"
    )
    status(response) must be (BAD_REQUEST)
    contentAsString(response) must (include("Not a valid unix handle") and
      include("Key comment &quot;;;;&quot; contains invalid characters"))
  }

  it must "reject registrations when selected handle is already taken" in new WithSampleSessions {
    val response = unregUser.submitForm("/register",
      "handle" -> regUserInGroup.handle,
      "publicKey" -> "ssh-rsa DKDJDJDK jsmith@example.com"
    )
    status(response) must be (BAD_REQUEST)
    contentAsString(response) must include ("already taken")
  }

  it must "reject registration when user is already being registered" in new WithSampleSessions {
    withPersistentHdfsDeployed {
      val runningTask = services.taskDao.registerTask()
      runningTask.resource = "newuser"
      runningTask.metadata = "registration"
      val response = unregUser.submitForm("/register",
        "handle" -> "newuser",
        "email" -> "jsmith@example.com",
        "publicKey" -> "ssh-rsa DKDJDJDK jsmith@example.com")
      status(response) must be (BAD_REQUEST)
      contentAsString(response) must include ("already running")
    }
  }

  "The profile page" must "show the user profile page for registered users" in new WithSampleSessions {
    val profilePage = regUserInGroup.doRequest("/profile")
    status(profilePage) must equal (OK)
    contentAsString(profilePage) must include (s"Profile for ${regUserInGroup.userProfile.contact}")
  }

  it must behave like enabledWhenUnderMaintenance(FakeRequest(GET, "/profile"))
  it must behave like pageForRegisteredUsers("/profile")

  "The getting started page" must "show a personalized getting started tutorial" in
    new WithSampleSessions {
      val response = regUserInGroup.doRequest("/getting-started")
      contentAsString(response) must (
        include (regUserInGroup.cosmosProfile.apiCredentials.apiKey) and
        include (regUserInGroup.cosmosProfile.apiCredentials.apiSecret))
    }

  it must behave like pageForRegisteredUsers("/getting-started")
  it must behave like enabledWhenUnderMaintenance(FakeRequest(GET, "/getting-started"))

  "The OAuth authorization resource" must behave like
    enabledWhenUnderMaintenance(FakeRequest(GET, "/auth/provider"))

  "A registered user" must "be authenticated after OAuth redirection" in
    new WithTestApplication {
      registerUser(MockAuthConstants.User101)
      val redirection = oauthRedirectionWithCode(MockAuthConstants.GrantedCode)
      redirection must redirectTo ("/profile")
      val cosmosSession: CosmosSession = session(redirection)
      cosmosSession must be ('authenticated)
    }

  "A user rejecting authorization" must "see error information" in new WithTestApplication {
    val result = oauthRedirectionWithError("unauthorized_client")
    status(result) must be (UNAUTHORIZED)
    contentAsString(result) must include ("Access denied")
  }

  "A user with an invalid token" must "see error information and lose her session" in
    new WithTestApplication {
      val result = oauthRedirectionWithCode("invalid")
      val cosmosSession: CosmosSession = session(result)
      cosmosSession must not be 'authenticated
      status(result) must be (UNAUTHORIZED)
      contentAsString(result) must include ("Authorization failed")
    }

  private def oauthRedirectionWithCode(grantedCode: String) = oauthRedirection(s"code=$grantedCode")

  private def oauthRedirectionWithError(error: String) = oauthRedirection(s"error=$error")

  private def oauthRedirection(queryString: String) =
    route(FakeRequest(GET, s"/auth/${MockAuthConstants.ProviderId}?$queryString")).get

  private def authenticationUrl(page: String) =
    """<a class="login" href="(.*?)">""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1)))

  private def cancelRegistrationUrl(page: String) =
    """<a href="(.*?)" class="btn cancel">""".r.findFirstMatchIn(page)
      .map(m => StringEscapeUtils.unescapeHtml4(m.group(1))).get
}
