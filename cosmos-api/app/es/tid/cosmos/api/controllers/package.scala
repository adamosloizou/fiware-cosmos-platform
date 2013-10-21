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

package es.tid.cosmos.api

import scala.language.implicitConversions

import es.tid.cosmos.api.controllers.profile.UserProfile
import es.tid.cosmos.api.profile.CosmosProfile
import es.tid.cosmos.servicemanager.ClusterUser

package object controllers {

  class ClusterProfileExtensions(profile: CosmosProfile) {
    val toClusterUser = ClusterUser(profile.handle, profile.keys.head.signature)
    val toUserProfile = UserProfile(profile.handle, profile.keys.toList)
  }

  implicit def extendMyProfile(profile: CosmosProfile) = new ClusterProfileExtensions(profile)
}
