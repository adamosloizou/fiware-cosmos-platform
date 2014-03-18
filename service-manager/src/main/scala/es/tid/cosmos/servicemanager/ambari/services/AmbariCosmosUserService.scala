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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.{Service, ComponentDescription}

object AmbariCosmosUserService extends AmbariServiceDetails {
  override val service: Service = CosmosUserService
  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.masterComponent("USER_MASTER_MANAGER").makeClient,
    ComponentDescription.slaveComponent("USER_SLAVE_MANAGER").makeClient
  )
}
