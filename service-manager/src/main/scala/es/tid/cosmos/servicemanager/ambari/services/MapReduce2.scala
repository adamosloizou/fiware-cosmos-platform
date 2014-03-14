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

import es.tid.cosmos.servicemanager.{NoParametrization, Service, ComponentDescription}

/** Representation of the Map Reduce 2 service.
  *
  * The service needs to run along with YARN.
  * @see [[es.tid.cosmos.servicemanager.ambari.services.Yarn]]
  */
object MapReduce2 extends Service with NoParametrization {
  override val name: String = "MAPREDUCE2"

  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.masterComponent("HISTORYSERVER"),
    ComponentDescription.masterComponent("MAPREDUCE2_CLIENT").makeClient
  )
  override val dependencies: Set[Service] = Set(Yarn)
}
