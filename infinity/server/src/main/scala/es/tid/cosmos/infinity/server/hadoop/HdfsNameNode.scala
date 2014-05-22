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

import java.util
import java.io.FileNotFoundException
import java.net.URL
import java.util.Date
import scala.concurrent._
import scala.util.Try

import org.apache.hadoop.fs.{CreateFlag, FileAlreadyExistsException, ParentNotDirectoryException}
import org.apache.hadoop.hdfs.protocol.{AlreadyBeingCreatedException, DirectoryListing, HdfsFileStatus}
import org.apache.hadoop.hdfs.server.protocol.NamenodeProtocols
import org.apache.hadoop.io.EnumSetWritable
import org.apache.hadoop.security.AccessControlException

import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.infinity.common.hadoop.HadoopConversions._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.server.config.MetadataServerConfig
import es.tid.cosmos.infinity.server.urls.UrlMapper

class HdfsNameNode(
    config: MetadataServerConfig,
    protocols: HdfsNameNode.NamenodeProtocolsLoaner,
    urlMapper: UrlMapper) extends NameNode {

  import ExecutionContext.Implicits.global

  def this(config: MetadataServerConfig, protocols: NamenodeProtocols, urlMapper: UrlMapper) =
    this(config, new HdfsNameNode.NamenodeProtocolsLoaner(protocols), urlMapper)

  override def pathMetadata(path: Path): Future[PathMetadata] = future {
    val fileStatus = checkedFileInfo(path)
    if (fileStatus.isDir) dirMetadataOf(path, fileStatus, directoryContents(path, fileStatus))
    else fileMetadataOf(path, fileStatus)
  }

  override def createFile(
      path: Path,
      owner: String,
      group: String,
      permissions: PermissionsMask,
      replication: Option[Short],
      blockSize: Option[Long]): Future[Unit] = future {
    protocols.forPath(path) { p =>
      p.create(
        path.toString, // src
        permissions.toHadoop,
        "hdfs", // TODO: determine what this parameter is used for
        new EnumSetWritable(util.EnumSet.of(CreateFlag.CREATE)),
        false, //createParent
        replication.getOrElse(config.replication),
        blockSize.getOrElse(config.blockSize))
    }
  }

  override def createDirectory(
      path: Path,
      owner: String,
      group: String,
      permissions: PermissionsMask): Future[Unit] = future {
    protocols.forPath(path) { p =>
      p.mkdirs(
        path.toString, // src
        permissions.toHadoop,
        false) // createParent
    }
  }

  override def deletePath(path: Path, recursive: Boolean): Future[Unit] = future {
    protocols.forPath(path) { p =>
      if (!p.delete(path.toString, recursive)) {
        throw new IllegalStateException("unexpected return value from NameNode.delete()")
      }
    }
  }

  override def movePath(from: Path, to: Path): Future[Unit] = future {
    requirePathExists(from)
    requirePathNotExists(to)
    protocols.forPath(from) { p =>
      if (!p.rename(from.toString, to.toString)) {
        throw new IllegalStateException("unexpected return value from NameNode.rename()")
      }
    }
  }

  override def setOwner(path: Path, newOwner: String): Future[Unit] =
    setOwnerAndGroup(path, newOwner, HdfsNameNode.UseSameGroup)

  override def setGroup(path: Path, newGroup: String): Future[Unit] =
    setOwnerAndGroup(path, HdfsNameNode.UseSameUser, newGroup)

  override def setPermissions(path: Path, permissions: PermissionsMask): Future[Unit] = future {
    protocols.forPath(path) { p =>
      p.setPermission(path.toString, permissions.toHadoop)
    }
  }

  private def checkedFileInfo(path: Path): HdfsFileStatus = protocols.forPath(path) { p =>
    Option(p.getFileInfo(path.toString)).getOrElse(throw NameNodeException.NoSuchPath(path))
  }

  private def directoryContents(path: Path, fileStatus: HdfsFileStatus) = {
    def getListing(start: Array[Byte]) = protocols.forPath(path) { p =>
      p.getListing(path.toString, start, false)
    }
    lazy val directoryListings: Stream[DirectoryListing] = getListing(HdfsFileStatus.EMPTY_NAME) #::
      directoryListings.takeWhile(_.hasMore).map(prev => getListing(prev.getLastName))
    directoryListings.flatMap(_.getPartialListing).map(s => (path / s.getLocalName, s)).toList
  }

  private def fileMetadataOf(path: Path, fileStatus: HdfsFileStatus) = FileMetadata(
    path,
    metadata = urlMapper.metadataUrl(path),
    content = pickContentServer(path, fileStatus),
    owner = fileStatus.getOwner,
    group = fileStatus.getGroup,
    modificationTime = new Date(fileStatus.getModificationTime),
    accessTime = new Date(fileStatus.getAccessTime),
    permissions = fileStatus.getPermission.toInfinity,
    replication = fileStatus.getReplication,
    blockSize = fileStatus.getBlockSize,
    size = fileStatus.getLen
  )

  private def dirMetadataOf(
      path: Path,
      fileStatus: HdfsFileStatus,
      contents: Seq[(Path, HdfsFileStatus)]) = DirectoryMetadata(
    path,
    metadata = urlMapper.metadataUrl(path),
    owner = fileStatus.getOwner,
    group = fileStatus.getGroup,
    modificationTime = new Date(fileStatus.getModificationTime),
    accessTime = new Date(fileStatus.getAccessTime),
    permissions = fileStatus.getPermission.toInfinity,
    content = contents.map(c => directoryEntryOf(c._1, c._2))
  )

  private def directoryEntryOf(path: Path, fileStatus: HdfsFileStatus) = {
    DirectoryEntry(
      path = path,
      `type` = if (fileStatus.isDir) PathType.Directory else PathType.File,
      metadata = urlMapper.metadataUrl(path),
      owner = fileStatus.getOwner,
      group = fileStatus.getGroup,
      modificationTime = new Date(fileStatus.getModificationTime),
      accessTime = new Date(fileStatus.getAccessTime),
      permissions = fileStatus.getPermission.toInfinity,
      replication = fileStatus.getReplication,
      blockSize = fileStatus.getBlockSize,
      size = fileStatus.getLen
    )
  }

  private def pickContentServer(path: Path, fileStatus: HdfsFileStatus): Option[URL] =
    protocols.forPath(path) { p =>
      val blocks = p.getBlockLocations(path.toString, 0, fileStatus.getLen)
      if (blocks.locatedBlockCount() != 0) {
        val locs = blocks.get(0).getLocations
        if (locs.size != 0) Some(urlMapper.contentUrl(path, locs(0).getHostName)) else None
      }
      else None
    }

  private def setOwnerAndGroup(path: Path, owner: String, group: String) = future {
    protocols.forPath(path) { p => p.setOwner(path.toString, owner, group) }
  }

  /** Check whether given path exists, throwing NoSuchPath if not. */
  private def requirePathExists(path: Path): Unit = checkedFileInfo(path)

  /** Check whether given path doesn't exist, throwing PathAlreadyExists if not. */
  private def requirePathNotExists(path: Path): Unit = if (Try(checkedFileInfo(path)).isSuccess) {
    throw NameNodeException.PathAlreadyExists(path)
  }
}

object HdfsNameNode {

  private val UseSameUser = null
  private val UseSameGroup = null

  private class NamenodeProtocolsLoaner(protocols: NamenodeProtocols) {

    def forPath[T](path: Path)(body: NamenodeProtocols => T): T = try {
      body(protocols)
    } catch {
      case e: AccessControlException => throw NameNodeException.Unauthorized(path, e)
      case e: AlreadyBeingCreatedException => throw NameNodeException.PathAlreadyExists(path, e)
      case e: FileNotFoundException => throw NameNodeException.NoSuchPath(path, e)
      case e: FileAlreadyExistsException => throw NameNodeException.PathAlreadyExists(path, e)
      case e: ParentNotDirectoryException => throw NameNodeException.ParentNotDirectory(path, e)
      case e: NameNodeException => throw e
      case e: Throwable => throw NameNodeException.IOError(e)
    }
  }
}
