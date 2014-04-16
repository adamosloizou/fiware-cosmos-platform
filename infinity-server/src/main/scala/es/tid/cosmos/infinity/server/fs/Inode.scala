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

package es.tid.cosmos.infinity.server.fs

import java.util.UUID

import es.tid.cosmos.infinity.server.authentication.UserProfile

sealed trait Inode {
  val id: String
  val permissions: FilePermissions
  val isDirectory: Boolean

  def canRead(user: UserProfile): Boolean = permissions.canRead(user)
  def canWrite(user: UserProfile): Boolean = permissions.canWrite(user)
  def canExec(user: UserProfile): Boolean = permissions.canExec(user)
}

sealed trait DirectoryInode extends Inode {

  override val isDirectory = true

  def newFile(name: String, permissions: FilePermissions = permissions) =
    FileInode(id = Inode.randomId(), parentId = id, name, permissions)

  def newDirectory(name: String, permissions: FilePermissions = permissions) =
    SubDirectoryInode(id = Inode.randomId(), parentId = id, name, permissions)
}

case class RootInode(permissions: FilePermissions) extends DirectoryInode {
  override val id: String = RootInode.Id
}
object RootInode {
  val Id = "0"
  val Name = "/"
}

sealed trait ChildInode extends Inode {
  val parentId: String
  val name: String
  require(id != parentId)
  require(!name.contains("/"))
}

case class FileInode(
    id: String, parentId: String, name: String, permissions: FilePermissions) extends ChildInode {
  override val isDirectory = false
}

case class SubDirectoryInode(
    id: String, parentId: String, name: String, permissions: FilePermissions)
  extends DirectoryInode with ChildInode

object Inode {
  def randomId() = UUID.randomUUID().toString.replace("-", "")
}
