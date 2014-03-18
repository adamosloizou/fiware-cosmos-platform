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

/** Representation of Hive service */
object Hive extends Service with NoParametrization {
  override val name: String = "HIVE"

  override val dependencies: Set[Service] = Set(Hdfs, MapReduce2, HCatalog, WebHCat)

  override val components: Seq[ComponentDescription] = Seq(
    ComponentDescription.masterComponent("HIVE_SERVER"),
    ComponentDescription.masterComponent("HIVE_METASTORE"),
    ComponentDescription.masterComponent("HIVE_CLIENT").makeClient,
    ComponentDescription.masterComponent("MYSQL_SERVER")
  )
}
