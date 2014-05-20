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

package es.tid.cosmos.infinity.server.hadoop

import es.tid.cosmos.infinity.common.permissions.{PermissionsMask, UserProfile}
import es.tid.cosmos.infinity.common.fs.{PathMetadata, Path}
import scala.concurrent.Future

class DummyNameNode extends NameNode {

  override def pathMetadata(path: Path): Future[PathMetadata] = ???

  override def createDirectory(
    path: Path, owner: String, group: String, permissions: PermissionsMask): Future[Unit] = ???

  override def createFile(
    path: Path, owner: String, group: String, permissions: PermissionsMask,
    replication: Option[Short], blockSize: Option[Long]): Future[Unit] = ???

  override def deletePath(path: Path, recursive: Boolean): Future[Unit] = ???

  override def movePath(from: Path, to: Path): Future[Unit] = ???

  override def setPermissions(path: Path, permissions: PermissionsMask): Future[Unit] = ???

  override def setGroup(path: Path, newGroup: String): Future[Unit] = ???

  override def setOwner(path: Path, newOwner: String): Future[Unit] = ???

  override def as[A](user: UserProfile)(body: => A): A = body
}
