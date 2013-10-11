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
import org.scalatest.matchers.MustMatchers
import play.api.http.Writeable
import play.api.test.FakeRequest
import play.api.test.Helpers._

import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.controllers.pages.WithSampleSessions
import es.tid.cosmos.api.controllers.ResultMatchers._
import es.tid.cosmos.api.mocks.WithSampleUsers

trait AuthBehaviors { this: FlatSpec with MustMatchers =>

  def rejectingUnauthenticatedRequests[T](request: FakeRequest[T])(
      implicit evidence: Writeable[T]
    ) {
    it must "reject unauthenticated request" in new WithSampleUsers {
      status(route(request).get) must equal (UNAUTHORIZED)
    }

    it must "reject request with malformed credentials" in new WithSampleUsers {
      val invalidRequest = request.withHeaders("Authorization" -> "invalid")
      status(route(invalidRequest).get) must equal (UNAUTHORIZED)
    }

    it must "reject request with invalid credentials"in new WithSampleUsers {
      val invalidCredentials = ApiCredentials.random()
      status(route(request.authorizedBy(invalidCredentials)).get) must equal (UNAUTHORIZED)
    }
  }

  def pageForRegistreredUsers(path: String) {
    it must "redirect unauthenticated users to the index" in new WithSampleSessions {
      unauthUser.doRequest(path) must redirectTo ("/")
    }

    it must "redirect unregistered users to the registration page" in new WithSampleSessions {
      unregUser.doRequest(path) must redirectTo ("/register")
    }
  }
}
