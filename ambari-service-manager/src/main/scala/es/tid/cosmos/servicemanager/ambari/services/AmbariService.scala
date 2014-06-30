/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.services.Service
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationContributor

/** Ambari-specific information about a service.
  *
  * Derived classes are expected to be objects.
  */
trait AmbariService {

  /** Associated Service */
  val service: Service

  val components: Seq[ComponentDescription]

  /** Build a configurator contributor for a given parametrization and configuration path */
  def configurator(
      parametrization: service.Parametrization, configPath: String): ConfigurationContributor

  /** The state of a service to be considered as running. */
  def runningState: ServiceState =
    if (components.forall(_.isClient)) InstalledService else StartedService
}