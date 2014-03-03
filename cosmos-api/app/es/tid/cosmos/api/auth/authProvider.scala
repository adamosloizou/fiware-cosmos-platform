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

package es.tid.cosmos.api.auth

trait AuthProvider {

  /** OAuth provider identifier aka authentication realm */
  def id: String

  /** Whether this auth provider has external user administration enabled. */
  def externalAdministrationEnabled: Boolean = adminPassword.isDefined

  /** Password for the admin resources */
  def adminPassword: Option[String]
}
