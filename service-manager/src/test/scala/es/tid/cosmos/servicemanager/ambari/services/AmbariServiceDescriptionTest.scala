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

import scala.concurrent.duration.Duration
import scala.concurrent.Await
import scala.concurrent.Future.successful

import org.mockito.BDDMockito.given
import org.mockito.Mockito.verify
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.ComponentDescription
import es.tid.cosmos.servicemanager.ambari.rest.{Service, Host, Cluster}
import es.tid.cosmos.servicemanager.ambari.configuration.{ConfigProperties, ConfigurationKeys, ConfigurationBundle}

class AmbariServiceDescriptionTest extends FlatSpec with MustMatchers with MockitoSugar {

  "An AmbariServiceDescription" must "create a service instance for a given cluster" in {
    val (cluster, master, slave, service) = (mock[Cluster], mock[Host], mock[Host], mock[Service])
    given(cluster.addService(Fake.name)).willReturn(successful(service))
    given(service.addComponent(Fake.component1.name)).willReturn(successful(Fake.component1.name))
    given(service.addComponent(Fake.component2.name)).willReturn(successful(Fake.component2.name))
    given(master.addComponents(Fake.component1.name)).willReturn(successful(()))
    given(slave.addComponents(Fake.component2.name)).willReturn(successful(()))
    val createdService = Await.result(Fake.createService(cluster, master, Seq(slave)), Duration.Inf)
    createdService must equal (service)
    verify(cluster).addService(Fake.name)
    verify(service).addComponent(Fake.component1.name)
    verify(service).addComponent(Fake.component2.name)
    verify(master).addComponents(Fake.component1.name)
    verify(slave).addComponents(Fake.component2.name)
  }

  it must "be considered running for state 'INSTALL' when its components are of client type" in {
    val description = new SimpleServiceDescription("component1" -> true, "component2" -> true)
    description.runningState must be (InstalledService)
  }

  it must "be considered running for state 'STARTED' in any other case" in {
    val description = new SimpleServiceDescription("component1" -> false, "component2" -> true)
    description.runningState must be (StartedService)
  }

  class SimpleServiceDescription(services: (String, Boolean)*) extends AmbariServiceDescription {
    def contributions(properties: ConfigProperties) =
      ConfigurationBundle(None, None, List())
    val name = "FakeServiceName"
    val components = for {
      (serviceName, isClient) <- services.toSeq
    } yield ComponentDescription.masterComponent(serviceName).copy(isClient = isClient)
  }

  object Fake extends AmbariServiceDescription {
    val name = "FakeServiceName"
    val component1 = ComponentDescription.masterComponent("component1")
    val component2 = ComponentDescription.slaveComponent("component2").makeClient
    val components: Seq[ComponentDescription] = Seq(component1, component2)
    override def contributions(properties: Map[ConfigurationKeys.Value, String]) =
      ConfigurationBundle(None, None, List())
  }
}
