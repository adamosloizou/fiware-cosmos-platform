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

import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys

class HiveIT extends ConfiguredServiceTest {

  override val dynamicProperties = Map(
    ConfigurationKeys.MasterNode -> "aMasterNodeName"
  )

  override def configurator = AmbariHive.configurator((), resourcesConfigDirectory)

  "The Hive service" must "have global, core and service configuration contributions" in {
    contributions.global must be ('defined)
    contributions.core must be ('defined)
    contributions.services must have length 1
  }
}
