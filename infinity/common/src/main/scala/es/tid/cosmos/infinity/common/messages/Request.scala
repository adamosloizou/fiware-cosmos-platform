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

package es.tid.cosmos.infinity.common.messages

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

/** Action that can be performed on a path. */
sealed trait Request {
  val action: String = Request.nameFor(getClass).getOrElse(throw new NoSuchElementException(
    s"Oops! Looks like the Request [${getClass.getSimpleName}] " +
      "is not included in the Names manifest map."))
}

object Request {

  lazy val Names: Map[Manifest[_ <: Request], String] = Map(
    manifest[CreateFile] -> "mkfile",
    manifest[CreateDirectory] -> "mkdir",
    manifest[Move] -> "move",
    manifest[ChangeOwner] -> "chown",
    manifest[ChangeGroup] -> "chgrp",
    manifest[ChangePermissions] -> "chmod"
  )

  def nameFor[T <: Request](implicit clazz: Class[T]): Option[String] = Names.collectFirst {
    case (mf, name) if mf.runtimeClass == clazz => name
  }

  def manifestFor(name: String): Option[Manifest[_ <: Request]] = Names.collectFirst {
    case (mf, `name`) => mf
  }

  case class CreateFile(
      name: String,
      permissions: PermissionsMask,
      replication: Option[Short],
      blockSize: Option[Long]) extends Request {

    Path.requireValidPathElement(name)
    replication.foreach(r => require(r > 0, s"Replication should be positive but was $r"))
    blockSize.foreach(bs => require(bs > 0, s"Block size should be positive but was $bs"))
  }

  case class CreateDirectory(name: String, permissions: PermissionsMask) extends Request {
    Path.requireValidPathElement(name)
  }

  case class Move(name: String, from: Path) extends Request {
    Path.requireValidPathElement(name)
  }

  case class ChangeOwner(owner: String) extends Request

  case class ChangeGroup(group: String) extends Request

  case class ChangePermissions(permissions: PermissionsMask) extends Request
}

