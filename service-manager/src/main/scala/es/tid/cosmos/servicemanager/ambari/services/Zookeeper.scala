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

/** Representation of the Zookeeper service. */
object Zookeeper extends Service with NoParametrization {
  override val name: String = "ZOOKEEPER"

  override val components: Seq[ComponentDescription] = Seq(
    /* Zookeeper is a distributed application.
     * We choose to have one zookeeper server on each slave for resilience purposes.
     */
    ComponentDescription.slaveComponent("ZOOKEEPER_SERVER"),
    ComponentDescription.masterComponent("ZOOKEEPER_CLIENT").makeClient
  )
}


