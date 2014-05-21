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

package es.tid.cosmos.infinity.server.metadata

import scala.util.Try

import unfiltered.request._

import es.tid.cosmos.infinity.common.fs.Path
import es.tid.cosmos.infinity.common.json.{ParseException, RequestMessageParser}
import es.tid.cosmos.infinity.common.messages.{Request => RequestMessage}
import es.tid.cosmos.infinity.server.actions._
import es.tid.cosmos.infinity.server.config.InfinityConfig
import es.tid.cosmos.infinity.server.errors.RequestParsingException
import es.tid.cosmos.infinity.server.hadoop.NameNode

/** An extractor object aimed to convert a Finagle HTTP request into a Infinity Server action. */
class HttpActionValidator(config: InfinityConfig, nameNode: NameNode) {

  private val jsonParser = new RequestMessageParser()
  private val metadataUriPrefix = {
    val base = config.metadataBasePath.replaceAll("/*$", "")
    s"""$base(/[^\\?]*)?(\\?.*)?""".r
  }

  def apply[T](request: HttpRequest[T]): Try[MetadataAction] = Try {
    request.uri match {
      case metadataUriPrefix(path, _) => metadataAction(Option(path).getOrElse("/"), request)
      case uri => throw RequestParsingException.InvalidResourcePath(uri)
    }
  }

  private def metadataAction[T](path: String, request: HttpRequest[T]) = {
    val absolutePath = Path.absolute(path)
    request match {
      case GET(_) =>
        GetMetadata(nameNode, absolutePath)
      case POST(_) =>
        postMetadataAction(absolutePath, request)
      case DELETE(Params(params)) =>
        DeletePath(nameNode, absolutePath, params("recursive").headOption.exists(_.toBoolean))
    }
  }

  private def postMetadataAction[T](absolutePath: Path, request: HttpRequest[T]) = {
    val content = Body.string(request)
    try {
      jsonParser.parse(content) match {
        case RequestMessage.CreateFile(name, perms, rep, bsize) =>
          CreateFile(config, nameNode, absolutePath / name, perms, rep, bsize)
        case RequestMessage.CreateDirectory(name, perms) =>
          CreateDirectory(nameNode, absolutePath / name, perms)
        case RequestMessage.ChangeOwner(owner) =>
          ChangeOwner(nameNode, absolutePath, owner)
        case RequestMessage.ChangeGroup(group) =>
          ChangeGroup(nameNode, absolutePath, group)
        case RequestMessage.ChangePermissions(permissions) =>
          ChangePermissions(nameNode, absolutePath, permissions)
        case RequestMessage.Move(name, from) =>
          MovePath(config, nameNode, absolutePath / name, from)
      }
    } catch {
      case e: ParseException =>
        throw RequestParsingException.InvalidRequestBody(content, e)
    }
  }
}
