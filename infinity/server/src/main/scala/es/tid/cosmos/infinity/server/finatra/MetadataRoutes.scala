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
package es.tid.cosmos.infinity.server.finatra

import java.net.InetAddress
import scala.concurrent.ExecutionContext.Implicits.global
import scalaz.Validation

import com.twitter.finatra.{Controller, Request}

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.authentication.AuthenticationService
import es.tid.cosmos.infinity.server.authorization.{AuthInfo, HttpCredentialsValidator}
import es.tid.cosmos.infinity.server.config.InfinityConfig
import es.tid.cosmos.infinity.server.errors.RequestParsingException
import es.tid.cosmos.infinity.server.hadoop.NameNode
import es.tid.cosmos.infinity.server.urls.UrlMapper
import es.tid.cosmos.infinity.server.util.TwitterConversions._

class MetadataRoutes(
    config: InfinityConfig,
    authService: AuthenticationService,
    nameNode: NameNode,
    urlMapper: UrlMapper) extends Controller {
  import MetadataRoutes._

  private val basePath = config.metadataBasePath
  private val actionValidator = new HttpActionValidator(config, nameNode)

  get(s"$basePath/*") { request =>
    val response = for {
      authInfo <- authorizationInfo(request.remoteAddress, request)
      credentials <- HttpCredentialsValidator(authInfo)
      action <- actionValidator(request)
    } yield for {
      profile <- authService.authenticate(credentials)
      context = Action.Context(profile, urlMapper)
      result <- action(context)
    } yield ActionResultHttpRenderer(result)
    response.fold(error => FinatraExceptionRenderer(error).toFuture, success => success.toTwitter)
  }

  error { request => request.error match {
    case Some(e) => FinatraExceptionRenderer(e).toFuture
    case None => FinatraExceptionRenderer(new IllegalStateException(
      "Finatra invoked the error handling routine but no error was found")).toFuture
  }}

  // `splat` is an undocumented Finatra tag. For wilcards routes extractions see
  // https://github.com/twitter/finatra/blob/master/src/main/scala/com/twitter/finatra/PathParser.scala
  private def getPath(request: Request): String = "/" + request.routeParams.getOrElse("splat", "")
}

private object MetadataRoutes {
  def authorizationInfo(
      from: InetAddress, request: Request): Validation[RequestParsingException, AuthInfo] = {
    import scalaz.Scalaz._
    Option(request.headers().get("Authorization"))
      .map(AuthInfo(from, _).success)
      .getOrElse(RequestParsingException.MissingAuthorizationHeader().failure)
  }
}
