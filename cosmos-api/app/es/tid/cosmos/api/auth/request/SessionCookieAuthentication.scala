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

package es.tid.cosmos.api.auth.request

import scalaz._

import play.api.mvc.RequestHeader

import es.tid.cosmos.api.controllers.pages.CosmosSession._
import es.tid.cosmos.api.profile.CosmosDao

/** Authenticates requests that have a valid session cookie associated. */
private[request] class SessionCookieAuthentication(dao: CosmosDao)
  extends RequestAuthentication {

  import Scalaz._

  /** Get the profile form the request session when possible. */
  override def authenticateRequest(request: RequestHeader): AuthResult =
    (for {
      userId <- request.session.userId
      profile <- dao.withTransaction { implicit c =>
        dao.profile.lookupByUserId(userId)
      }
    } yield profile.success).getOrElse(MissingAuthentication.failure)
}
