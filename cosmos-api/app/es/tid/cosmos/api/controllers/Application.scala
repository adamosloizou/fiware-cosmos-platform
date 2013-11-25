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

import play.api.mvc.Controller

import es.tid.cosmos.api.auth.MultiAuthProviderComponent
import es.tid.cosmos.api.controllers.admin.{MaintenanceStatusComponent, UserResource}
import es.tid.cosmos.api.controllers.cluster.ClusterResource
import es.tid.cosmos.api.controllers.cosmos.CosmosResource
import es.tid.cosmos.api.controllers.pages.{AdminPage, Pages}
import es.tid.cosmos.api.controllers.profile.ProfileResource
import es.tid.cosmos.api.controllers.services.ServicesResource
import es.tid.cosmos.api.controllers.storage.StorageResource
import es.tid.cosmos.api.profile.CosmosProfileDaoComponent
import es.tid.cosmos.servicemanager.ServiceManagerComponent

/**
 * Web application template to be mixed-in with its dependencies.
 */
abstract class Application {
  this: ServiceManagerComponent
    with MultiAuthProviderComponent
    with CosmosProfileDaoComponent
    with MaintenanceStatusComponent =>

  lazy val dao = this.cosmosProfileDao

  lazy val controllers: Map[Class[Controller], Controller] = {
    val status = this.maintenanceStatus
    val sm = this.serviceManager()
    val multiAuthProvider = this.multiAuthProvider
    controllerMap(
      new Pages(multiAuthProvider, sm, dao, status),
      new AdminPage(dao, status),
      new CosmosResource(),
      new ProfileResource(dao),
      new ClusterResource(sm, dao, status),
      new StorageResource(sm, dao, status),
      new ServicesResource(sm),
      new CliConfigResource(dao),
      new UserResource(multiAuthProvider, sm, dao, status)
    )
  }

  def services: ServiceManagerComponent
    with MultiAuthProviderComponent
    with MaintenanceStatusComponent = this

  private def controllerMap(controllers: Controller*) = Map(
    (for (controller <- controllers)
      yield controller.getClass.asInstanceOf[Class[Controller]] -> controller): _*)
}
