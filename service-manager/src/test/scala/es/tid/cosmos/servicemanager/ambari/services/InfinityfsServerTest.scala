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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.ComponentDescription

class InfinityfsServerTest extends FlatSpec with MustMatchers {

  "An infinity server service" must "have a single driver component" in {
    InfinityfsServer.components must be (Seq(
      ComponentDescription.masterComponent("INFINITY_HFS_MASTER_SERVER"),
      ComponentDescription.slaveComponent("INFINITY_HFS_SLAVE_PROXY")
    ))
  }
}
