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

package es.tid.cosmos.servicemanager.ambari.configuration

/** Trait for contributing cluster configuration.
  * A contributor can offer partial Global and Core configuration as well as the complete
  * configuration of a given Service.
  */
trait ConfigurationContributor {
  /** Get the configuration contributions.
    *
    * @param properties a map that contains the keys and values of cluster-dependent properties
    * @return the partial contribution to the cluster's Global and Core configuration and
    *         the complete configuration of a Service.
    */
  def contributions(properties: ConfigProperties): ConfigurationBundle
}
