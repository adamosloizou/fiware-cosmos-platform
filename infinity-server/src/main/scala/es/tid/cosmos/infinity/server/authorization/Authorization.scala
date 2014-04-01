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

package es.tid.cosmos.infinity.server.authorization

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication.UserProfile

object Authorization {

  /** A message requesting the authorization to execute an action on a path. */
  case class Authorize(action: Action, profile: UserProfile)

  sealed trait AuthorizationResponse

  /** A message responding with a success authorization. */
  case object Authorized extends AuthorizationResponse

  /** A message responding with a failed authorization. */
  case class AuthorizationFailed(error: Throwable) extends AuthorizationResponse
}
