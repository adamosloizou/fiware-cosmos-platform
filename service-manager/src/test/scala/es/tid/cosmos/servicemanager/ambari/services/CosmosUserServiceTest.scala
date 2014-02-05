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
import org.scalatest.matchers.{MatchResult, Matcher, MustMatchers}

import es.tid.cosmos.servicemanager.{ClusterUser, ComponentDescription}
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationKeys

class CosmosUserServiceTest extends FlatSpec with MustMatchers {

  val description = new CosmosUserService(Seq(ClusterUser(
    username = "userName",
    publicKey = "public_key",
    sshEnabled = true,
    hdfsEnabled = false,
    isSudoer = false
  )))
  val contributions = description.contributions(Map(
    ConfigurationKeys.MasterNode -> "aNameNodeName"
  ))

  "A Cosmos user service" must "have a master and a slave components" in {
    description.name must equal("COSMOS_USER")
    description.components must (
      have length 2 and
      contain(ComponentDescription("USER_MASTER_MANAGER", isMaster = true, isClient = true)) and
      contain(ComponentDescription("USER_SLAVE_MANAGER", isMaster = false, isClient = true))
    )
  }

  it must "not contribute to global and core configurations" in {
    contributions.global must be('empty)
    contributions.core must be('empty)
  }

  it must "contribute to service configurations" in {
    contributions.services must have length 1
    val properties = contributions.services(0).properties
    properties.get("user1_ssh_enabled") must be (Some(true))
    properties.get("user1_hdfs_enabled") must be (Some(false))
    properties.get("user1_ssh_master_authorized_keys") must containsSshKey("public_key")
    properties.get("user1_is_sudoer") must be (Some(false))
  }

  private def containsSshKey(key: String) = new Matcher[Option[Any]] {
    def apply(left: Option[Any]) = MatchResult(
      matches = left match {
        case Some(str: String) => str.contains(key)
        case _ => false
      },
      failureMessage = s"property value $left does not contains SSH key $key",
      negatedFailureMessage = s"property value $left contains SSH key $key"
    )
  }
}
