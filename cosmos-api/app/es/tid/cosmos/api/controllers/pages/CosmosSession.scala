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

import play.api.mvc.Session

import es.tid.cosmos.api.oauth2.UserProfile
import es.tid.cosmos.api.profile.UserId

/**
 * Enrich via implicit conversion the default Play! session object.
 * No other class have knowledge of the keys stored on the session.
 *
 * @constructor Wrap a session
 * @param s Wrapped session
 */
class CosmosSession(val s: Session) {
  def isAuthenticated: Boolean = userProfile.isDefined
  def isRegistered: Boolean = s.get("cosmosId").isDefined
  def cosmosId: Option[Long] = s.get("cosmosId").map(_.toLong)
  def setCosmosId(userId: Long): Session = s + ("cosmosId" -> userId.toString)
  def setCosmosId(maybeUserId: Option[Long]): Session =
    maybeUserId.map(userId => setCosmosId(userId)).getOrElse(s)
  def token: Option[String] = s.get("token")
  def setToken(token: String): Session = s + ("token", token)
  def userProfile: Option[UserProfile] = for {
    realm <- s.get("authRealm")
    id <- s.get("authId")
  } yield UserProfile(
    id=UserId(realm, id),
    name=s.get("name"),
    email=s.get("email")
  )
  def setUserProfile(profile: UserProfile): Session =
    Seq("name" -> profile.name, "email" -> profile.email)
      .foldLeft(s + ("authRealm", profile.id.realm) + ("authId", profile.id.id))(
        (s, tuple) => tuple match {
          case (key, Some(value)) => s + (key, value)
          case _ => s
        })
}

object CosmosSession {
  implicit def asCosmosSession(s: Session): CosmosSession = new CosmosSession(s)
}
