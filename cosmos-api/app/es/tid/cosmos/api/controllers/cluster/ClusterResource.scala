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

package es.tid.cosmos.api.controllers.cluster

import javax.ws.rs.PathParam
import scala.util.{Failure, Success, Try}

import com.wordnik.swagger.annotations._
import play.Logger
import play.api.Play.current
import play.api.db.DB
import play.api.libs.json._
import play.api.mvc.{RequestHeader, Action}

import es.tid.cosmos.api.controllers.common.{AuthController, Message}
import es.tid.cosmos.servicemanager.{ClusterId, ServiceManager}
import es.tid.cosmos.api.profile.CosmosProfileDao

/**
 * Resource that represents a single cluster.
 */
@Api(value = "/cosmos/cluster", listingPath = "/doc/cosmos/cluster",
  description = "Represents an existing or decommissioned cluster")
class ClusterResource(serviceManager: ServiceManager) extends AuthController {
  @ApiOperation(value = "Get cluster machines", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.cluster.ClusterDetails")
  @ApiErrors(Array(
    new ApiError(code = 404, reason = "When cluster ID is unknown")
  ))
  def listDetails(
      @ApiParam(value = "Cluster identifier", required = true,
        defaultValue = "00000000-0000-0000-0000-000000000000")
      @PathParam("id")
      id: String) = Action { implicit request =>
    Authenticated(request) { profile =>
      val clusterId = ClusterId(id)
      val owned = isOwnCluster(profile.id, clusterId)
      val description = serviceManager.describeCluster(clusterId)
      (owned, description) match {
        case (false, Some(_)) => unauthorizedAccessTo(clusterId)
        case (false, None) => notFound(clusterId)
        case (true, None) => {
          Logger.error(s"Cluster '$id' is on app database but not found in Service Manager")
          InternalServerError(Json.toJson(Message(s"Cluster '$id' not found")))
        }
        case (true, Some(description)) => Ok(Json.toJson(ClusterDetails(description)))
      }
    }
  }

  @ApiOperation(value = "Terminate cluster", httpMethod = "POST", notes = "No body is required",
    responseClass = "es.tid.cosmos.api.controllers.common.Message")
  @ApiErrors(Array(
    new ApiError(code = 500, reason = "When cluster does not exists or cannot be terminated")
  ))
  def terminate(
       @ApiParam(value = "Cluster identifier", required = true,
         defaultValue = "00000000-0000-0000-0000-000000000000")
       @PathParam("id")
       id: String) = Action { request =>
    Authenticated(request) { profile =>
      val clusterId = ClusterId(id)
      val clusterExists = serviceManager.describeCluster(clusterId).isDefined
      val owned = isOwnCluster(profile.id, clusterId)
      (owned, clusterExists) match {
        case (_, false) => notFound(clusterId)
        case (true, _) => Try(serviceManager.terminateCluster(ClusterId(id))) match {
          case Success(_) => Ok(Json.toJson(Message("Terminating cluster")))
          case Failure(ex) => InternalServerError(ex.getMessage)
        }
        case (false, _) => unauthorizedAccessTo(clusterId)
      }
    }
  }

  private def isOwnCluster(cosmosId: Long, cluster: ClusterId): Boolean =
    DB.withConnection { implicit c =>
      val ownedClusters = CosmosProfileDao.clustersOf(cosmosId)(DB.getConnection())
      ownedClusters.contains(cluster)
    }

  private def unauthorizedAccessTo(cluster: ClusterId) =
    Unauthorized(Json.toJson(Message(s"Cannot access cluster '$cluster'")))

  private def notFound(cluster: ClusterId) =
    NotFound(Json.toJson(Message(s"No cluster '$cluster' exists")))
}

object ClusterResource {
  def clusterUrl(id: ClusterId)(implicit request: RequestHeader): String =
    routes.ClusterResource.listDetails(id.toString).absoluteURL(secure = false)
}
