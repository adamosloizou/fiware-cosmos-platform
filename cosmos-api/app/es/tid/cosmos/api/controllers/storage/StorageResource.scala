/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.api.controllers.storage

import java.net.URI
import scala.concurrent.Await
import scala.concurrent.duration._
import scala.language.postfixOps

import com.wordnik.swagger.annotations.{ApiError, ApiErrors, ApiOperation, Api}
import play.api.libs.json.Json
import play.api.mvc.Action

import es.tid.cosmos.api.controllers.admin.MaintenanceStatus
import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.CosmosProfileDao
import es.tid.cosmos.servicemanager.ServiceManager

/**
 * Resource that represents the persistent HDFS shared by all clusters.
 */
@Api(value = "/cosmos/v1/storage", listingPath = "/doc/cosmos/v1/storage",
  description = "Represents the persistent storage shared by all clusters")
class StorageResource(
    serviceManager: ServiceManager,
    override val dao: CosmosProfileDao,
    override val maintenanceStatus: MaintenanceStatus)
  extends ApiAuthController with MaintenanceAwareController {

  private val UnavailableHdfsResponse =
    ServiceUnavailable(Json.toJson(ErrorMessage("persistent storage service not available")))

  /**
   * Returns HDFS connection details.
   */
  @ApiOperation(value = "Persistent storage connection details", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.storage.WebHdfsConnection")
  @ApiErrors(Array(
    new ApiError(code = 503, reason = "When service is under maintenance")
  ))
  def details() = Action { implicit request =>
    unlessResourceUnderMaintenance {
      withApiAuth(request) { profile =>
        (for {
          description <- serviceManager.describeCluster(serviceManager.persistentHdfsId)
          if description.nameNode_>.isCompleted
          webHdfsUri = toWebHdfsUri(Await.result(description.nameNode_>, 100 milliseconds))
        } yield Ok(Json.toJson(WebHdfsConnection(location = webHdfsUri, user = profile.handle))))
        .getOrElse(UnavailableHdfsResponse)
      }
    }
  }

  private def toWebHdfsUri(nameNode: URI): URI =
    new URI("webhdfs", null, nameNode.getHost, nameNode.getPort, null, null, null)
}
