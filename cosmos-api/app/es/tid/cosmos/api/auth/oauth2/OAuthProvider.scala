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

package es.tid.cosmos.api.auth.oauth2

import es.tid.cosmos.api.auth.AuthProvider
import scala.concurrent.Future

trait OAuthProvider extends AuthProvider {

  /** Human-readable name of the provider */
  def name: String

  /** Create an OAuth authentication URL that asks for the required scopes.
    *
    * @param redirectUrl  URI that the OAuth provider uses to return the control
    * @return             The URL
    */
  def authenticationUrl(redirectUrl: String): String

  /**Link to the sign up page when supported by the provider. */
  def newAccountUrl: Option[String]

  /** Request a token able to access the user profile.
    *
    * @param code         Authorization code
    * @param redirectUrl  URI that the OAuth provider uses to return the control
    * @return             Future access token or an OAuthException in case of error
    */
  def requestAccessToken(code: String, redirectUrl: String): Future[String]

  /** Request the user profile given an access token
    *
    * @param token  Access token
    * @return       Future user profile, an [[OAuthException]] in case of OAuth error or
    *               [[InvalidProfileException]] if the profile cannot be parsed.
    */
  def requestUserProfile(token: String): Future[OAuthUserProfile]
}
