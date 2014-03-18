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

import es.tid.cosmos.servicemanager.ComponentDescription
import es.tid.cosmos.servicemanager.services.Service

/** Temporal bridge to refactor the service manager.
  *
  * TODO: rename to AmbariService
  */
trait AmbariServiceDetails {

  /** Associated Service */
  val service: Service

  val components: Seq[ComponentDescription]

  /** The state of a service to be considered as running. */
  def runningState: ServiceState =
    if (components.forall(_.isClient)) InstalledService else StartedService
}
