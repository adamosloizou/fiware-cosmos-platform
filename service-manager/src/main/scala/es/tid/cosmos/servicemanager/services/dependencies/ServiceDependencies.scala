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

package es.tid.cosmos.servicemanager.services.dependencies

import es.tid.cosmos.servicemanager.services._
import es.tid.cosmos.servicemanager._

/** Inter-service dependencies. */
case class ServiceDependencies(mapping: Map[Service, DependencyType]) {

  def required(requiredServices: Service*): ServiceDependencies =
    withDependencies(requiredServices, Required)

  def optional(optionalServices: Service*): ServiceDependencies =
    withDependencies(optionalServices, Optional)

  private def withDependencies(services: Seq[Service], dependencyType: DependencyType) = {
    requireNotAlreadyMapped(requireNotRepeated(services))
    copy(mapping ++ services.zip(Stream.continually(dependencyType)))
  }

  private def requireNotRepeated(services: Seq[Service]): Set[Service] = {
    val uniqueServices = services.toSet
    require(uniqueServices.size == services.size,
      s"Repeated services in: ${services.mkString(", ")}")
    uniqueServices
  }

  private def requireNotAlreadyMapped(services: Set[Service]): Unit = {
    val alreadyMapped = mapping.keySet intersect services
    require(alreadyMapped.isEmpty,
      s"Adding services already depended upon: ${alreadyMapped.mkString(", ")}")
  }
}

object ServiceDependencies {

  val none = ServiceDependencies(Map.empty)

  def required(requiredServices: Service*): ServiceDependencies = none.required(requiredServices: _*)

  def optional(optionalServices: Service*): ServiceDependencies = none.optional(optionalServices: _*)

  private lazy val Dependencies = new ServiceDependencyMapping(ServiceCatalogue)

  def executionPlan(serviceInstances: Set[AnyServiceInstance]): Seq[AnyServiceInstance] = {

    /** Use the passed instance, pick a default one or throw */
    def toServiceInstance(service: Service): AnyServiceInstance =
      serviceInstances.find(_.service == service)
        .orElse(service.defaultInstance)
        .getOrElse(throw new RuntimeException(s"No parametrization found for service ${service.name}"))

    val requestedServices = serviceInstances.map(_.service)
    val serviceExecutionPlan = Dependencies.executionPlan(requestedServices)
    serviceExecutionPlan.map(toServiceInstance)
  }
}
