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

import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.platform.manager.ial.serverpool.ServerPoolInfrastructureProviderComponent
import es.tid.cosmos.servicemanager.ambari.AmbariServiceManagerComponent

/**
 * Application tied to real services.
 */
object ProductionApplication extends Application {
  private val components = new AmbariServiceManagerComponent with ServerPoolInfrastructureProviderComponent
  lazy val serviceManager = components.serviceManager
}
