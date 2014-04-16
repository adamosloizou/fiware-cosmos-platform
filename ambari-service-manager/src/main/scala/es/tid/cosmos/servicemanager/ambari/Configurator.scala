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

package es.tid.cosmos.servicemanager.ambari

import java.util.Date
import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.rest.Cluster
import es.tid.cosmos.servicemanager.configuration._
import es.tid.cosmos.servicemanager.configuration.FactoryTypes.Factory
import es.tid.cosmos.servicemanager.configuration.FactoryTypes.Implicits._

/** Class for consolidating configuration from multiple contributors and applying it to a cluster. */
object Configurator {
  private val empty = ConfigurationBundle(None, None, List())

  /**
   * Apply the configuration offered by the given contributors to the given cluster.
   * This is done by consolidating the different pieces of configuration offered by each contributor
   * to a single contribution set and then applying it to the cluster.
   *
   * @param cluster the cluster to apply the configuration to
   * @param properties the values used to resolve dynamic configuration properties
   * @param contributors the configuration contributors offering pieces of cluster configuration
   * @return the futures of the cluster configuration application
   */
  def applyConfiguration(
    cluster: Cluster,
    properties: ConfigProperties,
    contributors: Seq[ConfigurationContributor]): Future[ConfigurationBundle] = {
    val tag = timestampedTag()
    val configurationBundle = consolidateConfiguration(contributors, properties)
    Future.traverse(configurationBundle.configurations)(cluster.applyConfiguration(_, tag)).map(
      _ => configurationBundle)
  }

  private def consolidateConfiguration(
      contributors: Seq[ConfigurationContributor],
      properties: ConfigProperties): ConfigurationBundle =
    contributors.map(_.contributions(properties)).foldLeft(empty)(consolidate)

  private def consolidate(
    alreadyMerged: ConfigurationBundle, toMerge: ConfigurationBundle): ConfigurationBundle = {
      val ConfigurationBundle(globalLeft, coreLeft, services) = alreadyMerged
      val ConfigurationBundle(globalRight, coreRight, servicesToAdd) = toMerge
      ConfigurationBundle(
        merge[GlobalConfiguration](globalLeft, globalRight),
        merge[CoreConfiguration](coreLeft, coreRight),
        concatenateIfNoDuplicates(servicesToAdd, services))
    }

  private def merge[T <: Configuration : Factory](left: Option[T], right: Option[T]): Option[T] =
    (left, right) match {
      case (None, _) => right
      case (_, None) => left
      case (Some(leftValue), Some(rightValue)) =>
        val conflictingKeys = leftValue.properties.filterKeys(rightValue.properties.contains)
        if (conflictingKeys.isEmpty)
          Some(implicitly[Factory[T]].apply(leftValue.properties ++ rightValue.properties))
        else
          throw new ConfigurationConflict(s"Found keys in conflict: $conflictingKeys")
    }

  private def concatenateIfNoDuplicates(
    toAdd: List[ServiceConfiguration],
    target: List[ServiceConfiguration]): List[ServiceConfiguration] = {
      val typesInConflict = toAdd.filter(candidate =>
        target.exists(candidate.configType == _.configType)).map(_.configType)
      if (typesInConflict.isEmpty)
        toAdd ++ target
      else throw new ConfigurationConflict(
        s"Already found a service configuration for type [${typesInConflict.mkString(", ")}")
  }

  private def timestampedTag() = s"version${new Date().getTime}"

  class ConfigurationConflict(message: String) extends Exception(message)
}
