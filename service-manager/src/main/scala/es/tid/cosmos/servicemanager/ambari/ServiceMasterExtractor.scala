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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import es.tid.cosmos.servicemanager.{Service, ComponentDescription}
import es.tid.cosmos.servicemanager.ambari.rest.{Host, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDetails

/** Extractor that returns the master host for a given service, i.e., the host that contains the
  * master component of that service
  */
object ServiceMasterExtractor {

  def getServiceMaster(cluster: Cluster, service: AmbariServiceDetails): Future[Host] = {
    val masterComponent = findMasterComponent(service)
    for (hosts <- cluster.getHosts) yield findMasterHost(hosts, masterComponent)
      .getOrElse(throw ServiceMasterNotFound(cluster, service))
  }

  private def findMasterHost(hosts: Seq[Host], masterComponent: ComponentDescription) =
    hosts.find(_.getComponentNames.contains(masterComponent.name))

  private def findMasterComponent(service: AmbariServiceDetails) = {
    val masterComponentOption = service.components.find(_.isMaster)
    require(masterComponentOption != None, "ServiceDescription must contain a master component")
    masterComponentOption.get
  }

  case class ServiceMasterNotFound(cluster: Cluster, description: AmbariServiceDetails)
    extends Exception(
      s"Masternode not found for Service[${description.service.name}]-Cluster[${cluster.name}]")
}
