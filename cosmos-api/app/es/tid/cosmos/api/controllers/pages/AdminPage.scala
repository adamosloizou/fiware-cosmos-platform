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

import scalaz._

import play.api.mvc.{Action, Controller}

import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.{CosmosProfile, Capability, CosmosProfileDao}
import es.tid.cosmos.api.profile.Capability.Capability

class AdminPage(
    override val dao: CosmosProfileDao,
    override val maintenanceStatus: MaintenanceStatus
  ) extends Controller with PagesAuthController with MaintenanceAwareController {

  import Scalaz._

  def show = Action { implicit request =>
    for {
      _ <- requirePageNotUnderMaintenance()
      (userProfile, cosmosProfile) <- requireUserProfiles(request)
      _ <- requireCapability(cosmosProfile, Capability.IsOperator)
    } yield Ok(views.html.admin(
      underMaintenance = maintenanceStatus.underMaintenance,
      tabs = Navigation.operatorNavigation))
  }

  private def requireCapability(
      profile: CosmosProfile, capability: Capability): ActionValidation[Unit] =
    if (profile.capabilities.hasCapability(capability)) ().success
    else Unauthorized(views.html.unauthorized()).fail
}
