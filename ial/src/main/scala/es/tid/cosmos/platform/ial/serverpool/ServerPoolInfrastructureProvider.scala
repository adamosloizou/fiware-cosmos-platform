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

package es.tid.cosmos.platform.ial.serverpool

import scala.concurrent.Future
import scala.util.{Success, Failure, Try}

import es.tid.cosmos.platform.ial.{MachineState, MachineProfile, InfrastructureProvider, ResourceExhaustedException}

/**
 * An infrastructure provider supported by a server pool.
 *
 * @author apv
 */
class ServerPoolInfrastructureProvider(val dao: ServerPoolDao) extends InfrastructureProvider {

  def createMachines(
      namePrefix: String,
      profile: MachineProfile.Value,
      count: Int): Future[Seq[MachineState]] = {
    val machineIds = dao.availableMachinesWith(_.profile == profile).take(count).map(_.id)
    if (machineIds.length < count)
      Future.failed(ResourceExhaustedException(profile.toString, count, machineIds.length))
    else {
      Future.successful((machineIds.zip(0 to machineIds.length).map({
        case (id, idx) => {
          dao.setMachineAvailability(id, available = false)
          dao.setMachineName(id, s"$namePrefix$idx").get
        }
      })))
    }
  }

  def releaseMachines(machines: Seq[MachineState]): Future[Unit] =
    throw new UnsupportedOperationException("This method is not yet implemented")

  // TODO: fix me
  val rootSshKey: String =  ""
}
