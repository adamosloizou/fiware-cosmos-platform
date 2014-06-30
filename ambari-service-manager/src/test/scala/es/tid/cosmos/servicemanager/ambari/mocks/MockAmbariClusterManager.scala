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

package es.tid.cosmos.servicemanager.ambari.mocks

import scala.concurrent.Future

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.AmbariClusterManager
import es.tid.cosmos.servicemanager.ambari.rest.ServiceClient
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceFactory
import es.tid.cosmos.servicemanager.clusters.{ClusterDescription, ClusterId, ImmutableClusterDescription}
import es.tid.cosmos.servicemanager.configuration.{ConfigurationBundle, DynamicPropertiesFactory}

class MockAmbariClusterManager extends AmbariClusterManager(
    ambariServer = null,
    rootPrivateSshKey = "",
    configPath = "/tmp/null",
    serviceLookup = AmbariServiceFactory.lookup
  ) {

  override def deployCluster(
      clusterDescription: ImmutableClusterDescription,
      serviceDescriptions: Seq[AnyServiceInstance],
      dynamicProperties: DynamicPropertiesFactory): Future[ConfigurationBundle] =
    Future.successful(ConfigurationBundle.NoConfiguration)

  override def removeCluster(cluster: ClusterDescription): Future[Unit] = Future.successful()

  override def changeServiceConfiguration(
    clusterDescription: ImmutableClusterDescription,
    dynamicProperties: DynamicPropertiesFactory,
    serviceDescription: AnyServiceInstance): Future[Unit] = Future.successful()

  override def stopStartedServices(id: ClusterId):Future[Seq[ServiceClient]] =
    Future.successful(Seq())

  override def updateClusterServices(
    clusterDescription: ImmutableClusterDescription,
    newServiceInstances: Seq[AnyServiceInstance],
    oldServiceInstances: Seq[AnyServiceInstance],
    dynamicProperties: DynamicPropertiesFactory): Future[ConfigurationBundle] =
   Future.successful(ConfigurationBundle.NoConfiguration)

}
