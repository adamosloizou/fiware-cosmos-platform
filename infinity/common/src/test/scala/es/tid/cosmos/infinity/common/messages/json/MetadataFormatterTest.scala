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

package es.tid.cosmos.infinity.common.messages.json

import java.net.URL

import net.liftweb.json._
import net.liftweb.json.JsonDSL._
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.infinity.common.json.MetadataFormatter
import es.tid.cosmos.infinity.common.json.formats.Rfc822DateFormat
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class MetadataFormatterTest extends FlatSpec with MustMatchers {

  val formatter = new MetadataFormatter
  val dateParser = new Rfc822DateFormat()
  dateParser.setTimeZone(DefaultFormats.UTC)
  val modificationTime = dateParser.parse("2014-04-08T12:31:45+0100")
  val accessTime = dateParser.parse("2014-04-08T12:45:22+0100")

  "A metadata formatter" must "format file metadata as JSON" in {
    val metadata = FileMetadata(
      path = Path.absolute("/usr/gandalf/spells.txt"),
      metadata = new URL("http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt"),
      content = new URL("http://example.com/infinityfs/v1/content/usr/gandalf/spells.txt"),
      owner = "gandalf",
      group = "istari",
      permissions = PermissionsMask.fromOctal("600"),
      size = 45566918656L,
      modificationTime = modificationTime,
      accessTime = Some(accessTime),
      blockSize = 65536,
      replication = 3
    )

    parse(formatter.format(metadata)) must be (
      ("path" -> "/usr/gandalf/spells.txt") ~
        ("metadata" -> "http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt") ~
        ("content" -> "http://example.com/infinityfs/v1/content/usr/gandalf/spells.txt") ~
        ("owner" -> "gandalf") ~
        ("group" -> "istari") ~
        ("modificationTime" -> dateParser.format(modificationTime)) ~
        ("accessTime" -> dateParser.format(accessTime)) ~
        ("permissions" -> "600") ~
        ("replication" -> 3) ~
        ("blockSize" -> 65536) ~
        ("size" -> 45566918656L) ~
        ("type" -> "file")
    )
  }

  it must "format directory metadata as JSON" in {
    val metadata = DirectoryMetadata(
      path = Path.absolute("/usr/gandalf"),
      metadata = new URL("http://example.com/infinityfs/v1/metadata/usr/gandalf"),
      owner = "gandalf",
      group = "istari",
      permissions = PermissionsMask.fromOctal("755"),
      modificationTime = modificationTime,
      content = Seq(
        DirectoryEntry.file(
          path = Path.absolute("/usr/gandalf/spells.txt"),
          metadata = new URL("http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt"),
          owner = "gandalf",
          group = "istari",
          modificationTime = modificationTime,
          accessTime = accessTime,
          permissions = PermissionsMask.fromOctal("600"),
          replication = 3,
          blockSize = 2048,
          size = 45566918656L
        ),
        DirectoryEntry.directory(
          path = Path.absolute("/usr/gandalf/enemies"),
          metadata = new URL("http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies"),
          owner = "gandalf",
          group = "istari",
          modificationTime = modificationTime,
          permissions = PermissionsMask.fromOctal("750")
        )
      )
    )

    parse(formatter.format(metadata)) must be (
      ("path" -> "/usr/gandalf") ~
        ("metadata" -> "http://example.com/infinityfs/v1/metadata/usr/gandalf") ~
        ("content" -> List(
          ("path" -> "/usr/gandalf/spells.txt") ~
            ("type" -> "file") ~
            ("metadata" -> "http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt") ~
            ("owner" -> "gandalf") ~
            ("group" -> "istari") ~
            ("modificationTime" -> dateParser.format(modificationTime)) ~
            ("accessTime" -> dateParser.format(accessTime)) ~
            ("permissions" -> "600") ~
            ("replication" -> 3) ~
            ("blockSize" -> 2048) ~
            ("size" -> 45566918656L),
          ("path" -> "/usr/gandalf/enemies") ~
            ("type" -> "directory") ~
            ("metadata" -> "http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies") ~
            ("owner" -> "gandalf") ~
            ("group" -> "istari") ~
            ("modificationTime" -> dateParser.format(modificationTime)) ~
            ("permissions" -> "750") ~
            ("replication" -> 0) ~
            ("blockSize" -> 0) ~
            ("size" -> 0)
        )) ~
        ("owner" -> "gandalf") ~
        ("group" -> "istari") ~
        ("modificationTime" -> dateParser.format(modificationTime)) ~
        ("permissions" -> "755") ~
        ("type" -> "directory") ~
        ("size" -> 0)
    )
  }
}
