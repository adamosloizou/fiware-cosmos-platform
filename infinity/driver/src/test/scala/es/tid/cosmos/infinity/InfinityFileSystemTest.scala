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

package es.tid.cosmos.infinity

import java.io.{FileNotFoundException, IOException}
import java.net.{URI, URL}
import java.util.Date

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs.Path
import org.apache.hadoop.fs.permission.FsPermission
import org.apache.hadoop.security.UserGroupInformation
import org.apache.hadoop.util.Progressable
import org.apache.log4j.BasicConfigurator
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.fs.{DirectoryEntry, DirectoryMetadata, FileMetadata, RootPath}
import es.tid.cosmos.infinity.common.hadoop.HadoopConversions._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.common.credentials.Credentials

class InfinityFileSystemTest extends FlatSpec with MustMatchers with MockitoSugar {

  BasicConfigurator.configure()

  "An infinity file system" must "have 'infinity' schema and uri" in new Fixture {
    fs.getScheme must be ("infinity")
    fs.getUri.getScheme must be ("infinity")
  }

  it must "use a default authority when missing" in new Fixture(uri = URI.create("infinity:///")) {
    fs.getUri.toString must be ("infinity://defaultAuthority/")
  }

  it must "know the home directory" in new Fixture {
    val username = UserGroupInformation.getCurrentUser.getUserName
    fs.getHomeDirectory must be (new Path(s"infinity://localhost:8888/user/$username"))
  }

  it must "have the home directory as initial working directory" in new Fixture {
    fs.getWorkingDirectory must be (fs.getHomeDirectory)
  }

  it must "update the working directory" in new Fixture {
    fs.setWorkingDirectory(new Path("/some/dir"))
    fs.getWorkingDirectory must be (new Path("infinity://localhost:8888/some/dir"))
  }

  it must "do nothing when asked to create the root path" in new Fixture {
    fs.mkdirs(new Path("/")) must be (true)
  }

  it must "create a directory whose parent exists" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/parent/newdir")
    client.givenDirectory(path.getParent.toInfinity)
    client.givenDirectoryCanBeCreated(path.toInfinity)

    fs.mkdirs(path, perms) must be (true)

    client.verifyDirectoryCreation(path.toInfinity, perms.toInfinity)
  }

  it must "create multiple directories at once" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/grandparent/parent/newdir")
    client.givenDirectory(path.getParent.getParent.toInfinity)
    client.givenDirectoryCanBeCreated(path.getParent.toInfinity)
    client.givenDirectoryCanBeCreated(path.toInfinity)

    fs.mkdirs(path, perms) must be (true)

    client.verifyDirectoryCreation(path.getParent.toInfinity, perms.toInfinity)
    client.verifyDirectoryCreation(path.toInfinity, perms.toInfinity)
  }

  it must "report directory creation failure" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/parent/newdir")
    client.givenDirectory(path.getParent.toInfinity)
    client.givenNonExistingPath(path.toInfinity)
    client.givenDirectoryCreationWillFail(path.toInfinity)

    fs.mkdirs(path, perms) must be (false)

    client.verifyDirectoryCreation(path.toInfinity, perms.toInfinity)
  }

  it must "fail if directory creation times out" in new Fixture {
    val perms = new FsPermission("755")
    val path = new Path("/parent/newdir")
    client.givenDirectory(path.getParent.toInfinity)
    client.givenNonExistingPath(path.toInfinity)
    client.givenDirectoryCreationWontFinish(path.toInfinity)

    fs.mkdirs(path, perms) must be (false)

    client.verifyDirectoryCreation(path.toInfinity, perms.toInfinity)
  }

  it must "get the file status of an existing file" in new Fixture {
    client.givenExistingPath(someFileMetadata)
    fs.getFileStatus(someFile) must be (someFileMetadata.toHadoop)
  }

  it must "get the file status of an existing directory" in new Fixture {
    client.givenExistingPath(someDirMetadata)
    fs.getFileStatus(someDir) must be (someDirMetadata.toHadoop)
  }

  it must "throw FileNotFoundException for non existing paths" in new Fixture {
    val path = new Path("/non/existing")
    client.givenNonExistingPath(path.toInfinity)
    evaluating {
      fs.getFileStatus(path)
    } must produce [FileNotFoundException]
  }

  it must "list directory status for files" in new Fixture {
    client.givenExistingPath(someFileMetadata)
    fs.listStatus(someFile) must be (Array(someFileMetadata.toHadoop))
  }

  it must "list status of children paths for directories" in new Fixture {
    client.givenExistingPath(someDirMetadata)
    fs.listStatus(someDir) must be (someDirMetadata.content.map(_.toHadoop).toArray)
  }

  it must "throw FileNotFoundException when listing non existing paths" in new Fixture {
    val path = new Path("/cannot/list/you")
    client.givenNonExistingPath(path.toInfinity)
    evaluating {
      fs.listStatus(path)
    } must produce [FileNotFoundException]
  }

  it must "not try to delete the root path" in new Fixture {
    fs.delete(new Path("/"), recursive = false) must be (false)
  }

  it must "delete subpaths" in new Fixture {
    val path = new Path("/subdir")
    val recursive = true
    client.givenCanBeDeleted(path.toInfinity)
    fs.delete(path, recursive) must be (true)
    client.verifyDeletion(path.toInfinity, recursive)
  }

  it must "return false on failure" in new Fixture {
    val path = new Path("/subdir")
    val recursive = false
    client.givenDeletionWillFail(path.toInfinity)
    fs.delete(path, recursive) must be (false)
    client.verifyDeletion(path.toInfinity, recursive)
  }

  it must "rename paths" in new Fixture {
    val source = new Path("/some/path")
    val target = new Path("/some/renamed")
    client.givenCanBeMoved(source.toInfinity, target.toInfinity)
    fs.rename(source, target) must be (true)
    client.verifyMove(source.toInfinity, target.toInfinity)
  }

  it must "return false when paths fail to be renamed" in new Fixture {
    val source = new Path("/some/path")
    val target = new Path("/some/renamed")
    client.givenMoveWillFail(source.toInfinity, target.toInfinity)
    fs.rename(source, target) must be (false)
    client.verifyMove(source.toInfinity, target.toInfinity)
  }

  it must "fail to rename the root directory" in new Fixture {
    fs.rename(new Path("/"), new Path("/other")) must be (false)
  }

  it must "do nothing when setting owner or group to null" in new Fixture {
    fs.setOwner(somePath, null, null)
    client.verifyNotChangedOwner(somePath.toInfinity)
    client.verifyNotChangedGroup(somePath.toInfinity)
  }

  it must "change path owner" in new Fixture {
    client.givenOwnerCanBeChanged(somePath.toInfinity)
    fs.setOwner(somePath, "gandalf", null)
    client.verifyOwnerChange(somePath.toInfinity, "gandalf")
    client.verifyNotChangedGroup(somePath.toInfinity)
  }

  it must "change path group" in new Fixture {
    client.givenGroupCanBeChanged(somePath.toInfinity)
    fs.setOwner(somePath, null, "istari")
    client.verifyNotChangedOwner(somePath.toInfinity)
    client.verifyGroupChange(somePath.toInfinity, "istari")
  }

  it must "throw IOException if owner change fail" in new Fixture {
    client.givenOwnerChangeWillFail(somePath.toInfinity)
    client.givenGroupCanBeChanged(somePath.toInfinity)
    evaluating {
      fs.setOwner(somePath, "gandalf", "istari")
    } must produce [IOException]
    client.verifyOwnerChange(somePath.toInfinity, "gandalf")
    client.verifyNotChangedGroup(somePath.toInfinity)
  }

  it must "throw IOException if group change fail" in new Fixture {
    client.givenOwnerCanBeChanged(somePath.toInfinity)
    client.givenGroupChangeWillFail(somePath.toInfinity)
    evaluating {
      fs.setOwner(somePath, "gandalf", "istari")
    } must produce [IOException]
    client.verifyOwnerChange(somePath.toInfinity, "gandalf")
    client.verifyGroupChange(somePath.toInfinity, "istari")
  }

  it must "change path permissions" in new Fixture {
    client.givenMaskCanBeChanged(somePath.toInfinity)
    val mask = PermissionsMask.fromOctal("750")
    fs.setPermission(somePath, mask.toHadoop)
    client.verifyMaskChange(somePath.toInfinity, mask)
  }

  it must "throw IOError when failing to change path permissions" in new Fixture {
    client.givenMaskChangeWillFail(somePath.toInfinity)
    val mask = PermissionsMask.fromOctal("750")
    evaluating {
      fs.setPermission(somePath, mask.toHadoop)
    } must produce [IOException]
    client.verifyMaskChange(somePath.toInfinity, mask)
  }

  it must "open an existing file" in new Fixture {
    client.givenExistingPath(someFileMetadata)
    fs.open(someFile) must not be null
  }

  it must "throw FileNotFoundException when opening a non existing file" in new Fixture {
    client.givenNonExistingPath(someFile.toInfinity)
    evaluating {
      fs.open(someFile)
    } must produce [FileNotFoundException]
  }

  it must "throw IOException when opening a directory" in new Fixture {
    client.givenExistingPath(someDirMetadata)
    evaluating {
      fs.open(someDir)
    } must produce [IOException]
  }

  it must "throw IOException when opening a file without available content URL" in new Fixture {
    client.givenExistingPath(someFileMetadata.copy(content = None))
    evaluating {
      fs.open(someFile)
    } must produce [IOException]
  }

  it must "append to an existing file" in new Fixture {
    client.givenFileCanBeAppendedTo(someFile.toInfinity)
    fs.append(someFile, 4096, null) must not be null
  }

  it must "throw FileNotFoundException when appending to a non existing file" in new Fixture {
    client.givenNonExistingPath(someFile.toInfinity)
    evaluating {
      fs.append(someFile, 4096, null)
    } must produce [FileNotFoundException]
  }

  it must "throw IOException when appending to a directory" in new Fixture {
    client.givenExistingPath(someDirMetadata)
    evaluating {
      fs.append(someDir, 4096, null)
    } must produce [IOException]
  }

  it must "create a new file" in new Fixture {
    client.givenFileCanBeCreated(someFile.toInfinity)
    client.givenFileCanBeAppendedTo(someFile.toInfinity)
    val mask = PermissionsMask.fromOctal("640")
    val overwrite = false
    val outputStream = fs.create(someFile, mask.toHadoop, overwrite, bufferSize, replication,
      blockSize, progressable)
    outputStream must not be null
    client.verifyFileCreation(someFile.toInfinity, mask, Some(replication), Some(blockSize))
  }

  it must "reject creating the root directory" in new Fixture {
    val mask = PermissionsMask.fromOctal("640")
    val overwrite = false
    evaluating {
      fs.create(new Path("/"), mask.toHadoop, overwrite, bufferSize, replication, blockSize, progressable)
    } must produce [IOException]
    client.verifyNoFileWasCreated()
  }

  val somePath = new Path("/some/path")
  val someTime = new Date(3600000L)
  val someFile = new Path("/some/file")
  val someFileMetadata = FileMetadata(
    path = someFile.toInfinity,
    metadata = new URL("http://metadata/some/file"),
    content = Some(new URL("http://content/some/file")),
    owner = "user",
    group = "cosmos",
    modificationTime = new Date(3600000L),
    accessTime = new Date(3600000L),
    permissions = PermissionsMask.fromOctal("640"),
    replication = 2,
    blockSize = 1024,
    size = 0
  )
  val someDir = new Path("/dir")
  val someDirMetadata = DirectoryMetadata(
    path = someDir.toInfinity,
    metadata = new URL("http://metadata/dir"),
    content = Seq(
      DirectoryEntry.file(
        path = RootPath / "dir" / "file",
        metadata = new URL("http://metadata/dir/file"),
        owner = "user",
        group = "cosmos",
        modificationTime = someTime,
        accessTime = someTime,
        permissions = PermissionsMask.fromOctal("700"),
        replication = 3,
        blockSize = 1024,
        size = 127
      ),
      DirectoryEntry.directory(
        path = RootPath / "dir" / "tmp",
        metadata = new URL("http://metadata/dir/tmp"),
        owner = "user",
        group = "cosmos",
        modificationTime = someTime,
        accessTime = someTime,
        permissions = PermissionsMask.fromOctal("1777")
      )
    ),
    owner = "user",
    group = "cosmos",
    modificationTime = someTime,
    accessTime = someTime,
    permissions = PermissionsMask.fromOctal("640")
  )
  val bufferSize = 2048
  val replication: Short = 3
  val blockSize = 100000L
  val progressable = new Progressable {
    override def progress(): Unit = {}
  }

  abstract class Fixture(uri: URI = URI.create("infinity://localhost:8888/")) {
    val client = new MockInfinityClient
    object ClientFactory extends InfinityClientFactory{
      override def build(metadataEndpoint: URL, credentials: Credentials) = client.value
    }
    val conf = new Configuration(false)
    conf.setInt(InfinityConfiguration.TimeoutProperty, 200)
    conf.set(InfinityConfiguration.DefaultAuthorityProperty, "defaultAuthority")
    conf.set(InfinityConfiguration.ClusterSecretProperty, "secret")
    val fs = new InfinityFileSystem(ClientFactory)
    fs.setConf(conf)
    fs.initialize(uri, conf)
  }
}
