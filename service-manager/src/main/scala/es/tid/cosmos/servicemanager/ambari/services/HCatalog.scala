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

import es.tid.cosmos.servicemanager.{ServiceDescription, ComponentDescription}

/** Representation of the HCatalog service */
object HCatalog extends ServiceDescription {
  override val name: String = "HCATALOG"

  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.masterComponent("HCAT").makeClient
  )
}
