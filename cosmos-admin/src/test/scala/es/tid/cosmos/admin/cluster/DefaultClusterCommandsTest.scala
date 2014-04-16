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

package es.tid.cosmos.admin.cluster

import java.net.URI
import scala.concurrent.Future

import org.mockito.BDDMockito.given
import org.mockito.Mockito.{verify, never}
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.clusters._

class DefaultClusterCommandsTest extends FlatSpec with MustMatchers with MockitoSugar {

  val clusterId = ClusterId("booya")

  trait WithServiceManager {
    val sm = mock[ServiceManager]
    val commands = new DefaultClusterCommands(sm)
  }

  trait WithMissingStorage extends WithServiceManager {
    given(sm.describeCluster(clusterId)).willReturn(None)
  }

  trait WithExistingStorage extends WithServiceManager {
    given(sm.describeCluster(clusterId)).willReturn(Some(new ImmutableClusterDescription(
      id = clusterId,
      name = ClusterName(""),
      state = Running,
      size = 3,
      nameNode = Some(new URI("hdfs://host:1234")),
      master = Some(HostDetails("host", "ipAddress")),
      slaves  = Seq(HostDetails("host2", "ipAddress2"), HostDetails("host3", "ipAddress3")),
      users = None,
      services = Set("HDFS"),
      blockedPorts = Set(2, 4, 6)
    )))
  }

  it must "not terminate the cluster if it hasn't been found" in new WithMissingStorage {
    commands.terminate(clusterId) must not be 'success
    verify(sm).describeCluster(clusterId)
    verify(sm, never()).terminateCluster(clusterId)
  }

  it must "terminate the cluster if it has been found" in new WithExistingStorage {
    given(sm.terminateCluster(clusterId)).willReturn(Future.successful())
    commands.terminate(clusterId) must be ('success)
    verify(sm).describeCluster(clusterId)
  }
}
