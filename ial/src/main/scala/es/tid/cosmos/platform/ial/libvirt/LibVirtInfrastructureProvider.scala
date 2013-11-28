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

package es.tid.cosmos.platform.ial.libvirt

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.{Future, future}

import es.tid.cosmos.platform.common.{ExecutableValidation, SequentialOperations}
import es.tid.cosmos.platform.ial._

/**
 * An infrastructure provider implemented on top of libvirt
 */
class LibVirtInfrastructureProvider(
    val dao: LibVirtDao,
    val libvirtServerFactory: LibVirtServerProperties => LibVirtServer,
    val rootPrivateSshKey: String)
  extends InfrastructureProvider {

  private val createMachinesSequencer = new SequentialOperations
  override def createMachines(
      preConditions: ExecutableValidation,
      profile: MachineProfile.Value,
      numberOfMachines: Int,
      bootstrapAction: MachineState => Future[Unit]): Future[Seq[MachineState]] = {

    createMachinesSequencer enqueue {
      preConditions().fold(
        fail = errors => throw PreconditionsNotMetException(profile, numberOfMachines, errors.list),
        succ = _ => proceedToMachineCreation(profile, numberOfMachines, bootstrapAction)
      )
    }
  }

  private def proceedToMachineCreation(
      profile: MachineProfile.Value,
      numberOfMachines: Int,
      bootstrapAction: MachineState => Future[Unit]): Future[Seq[MachineState]] = {

    val serversOfProfile = dao.libVirtServers.filter(_.profile == profile).map(libvirtServerFactory)
    (for {
      servers <- LibVirtServer.placeServers(serversOfProfile, numberOfMachines)
      machines <- createMachines(bootstrapAction, servers)
    } yield machines) recover {
      case error @ LibVirtServer.PlacementException(_, available) =>
        throw ResourceExhaustedException(profile.toString, numberOfMachines, available, error)
    }
  }

  override def releaseMachines(machines: Seq[MachineState]): Future[Unit] =
    for {
      servers <- serversForMachines(machines)
      _ <- Future.sequence(servers.map(srv => srv.destroyDomain()))
    } yield ()

  override def availableMachineCount(profile: MachineProfile.Value): Future[Int] =
    for {
      servers <- availableServers(profile)
    } yield servers.size

  override def machinePoolCount(profileFilter: MachineProfile.Value => Boolean): Int =
    dao.libVirtServers.map(_.profile).count(profileFilter)

  override def assignedMachines(hostNames: Seq[String]): Future[Seq[MachineState]] = {
    for {
      servers <- servers(srv => hostNames.contains(srv.domainHostname))
      machines <- machinesFromServers(servers)
    } yield machines
  }

  private def machinesFromServers(servers: Seq[LibVirtServer]): Future[Seq[MachineState]] =
    Future.traverse(servers)( srv =>
      for {
        domain <- srv.domain()
      } yield domainToMachineState(srv, domain)
    )

  private def serversForMachines(machines: Seq[MachineState]): Future[Seq[LibVirtServer]] = future {
    for {
      machine <- machines
      server <- dao.libVirtServers if server.domainHostname == machine.hostname
    } yield libvirtServerFactory(server)
  }

  private def createMachines(
      bootstrapAction: MachineState => Future[Unit],
      servers: Seq[LibVirtServer]): Future[Seq[MachineState]] =
    Future.sequence(for {
      server <- servers
    } yield createMachine(server, bootstrapAction))

  private def createMachine(srv: LibVirtServer, bootstrapAction: MachineState => Future[Unit]) =
    for {
      domain <- srv.createDomain()
      state = domainToMachineState(srv, domain)
      _      <- bootstrapAction(state)
    } yield state

  private def servers(
      pred: LibVirtServerProperties => Boolean): Future[Seq[LibVirtServer]] = future {
    for {
      serverProps <- dao.libVirtServers
      if pred(serverProps)
    } yield libvirtServerFactory(serverProps)
  }

  private def availableServers(
      pred: LibVirtServerProperties => Boolean): Future[Seq[LibVirtServer]] = {

    def availableServer(srv: LibVirtServer): Future[Option[LibVirtServer]] =
      for (created <- srv.isCreated()) yield if (created) None else Some(srv)

    val servers = for {
      serverProps <- dao.libVirtServers if pred(serverProps)
    } yield libvirtServerFactory(serverProps)
    Future.sequence(servers.map(availableServer)).map(_.flatten)
  }

  private def availableServers(profile: MachineProfile.Value): Future[Seq[LibVirtServer]] =
    availableServers(srv => srv.profile == profile)

  private def domainToMachineState(server: LibVirtServer, domain : DomainProperties): MachineState =
    new MachineState(
      Id(domain.uuid),
      domain.name,
      domain.profile,
      domainStatus(domain),
      domain.hostname,
      domain.ipAddress)

  private def domainStatus(dom: DomainProperties): MachineStatus.Value =
    if (dom.isActive) MachineStatus.Running else MachineStatus.Provisioning
}
