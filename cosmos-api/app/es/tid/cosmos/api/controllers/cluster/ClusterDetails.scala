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


import scala.Some

import com.wordnik.swagger.annotations.ApiProperty
import play.api.libs.json._
import play.api.mvc.RequestHeader

import es.tid.cosmos.servicemanager.clusters.{HostDetails, ClusterDescription}
import es.tid.cosmos.servicemanager.ClusterUser

/**
 * A cluster from the perspective of API clients.
 */
case class ClusterDetails(
    href: String,
    id: String,
    name: String,
    size: Int,
    @ApiProperty(allowableValues = "provisioning,running,terminating,terminated,failed")
    state: String,
    stateDescription: String,
    master: Option[HostDetails],
    slaves: Option[Seq[HostDetails]],
    users: Option[Seq[ClusterUser]]
)

object ClusterDetails {
  /**
   * Create a ClusterDetails from a description in the context of a request.
   *
   * @param desc     Cluster description
   * @param request  Context request
   * @return         A ClusterDetails instance
   */
  def apply(desc: ClusterDescription)(implicit request: RequestHeader): ClusterDetails =
    ClusterDetails(
      href = ClusterResource.clusterUrl(desc.id),
      id = desc.id.toString,
      name = desc.name,
      size = desc.size,
      state = desc.state.name,
      stateDescription = desc.state.descLine,
      master = desc.master,
      slaves = Option(desc.slaves),
      users = desc.users.map(_.toSeq)
    )

  implicit object HostDetailsWrites extends Writes[HostDetails] {
    def writes(info: HostDetails): JsValue = Json.obj(
        "hostname" -> info.hostname,
        "ipAddress" -> info.ipAddress
      )
  }

  implicit object ClusterUserWrites extends Writes[ClusterUser] {
    def writes(user: ClusterUser): JsValue =
      if (user.sshEnabled) {
        Json.obj(
          "username" -> user.username,
          "sshPublicKey" -> user.publicKey,
          "isSudoer" -> user.isSudoer
        )
    } else { Json.obj() }
  }

  implicit object ClusterDetailsWrites extends Writes[ClusterDetails] {
    def writes(d: ClusterDetails): JsValue = basicInfo(d) ++ machinesInfo(d) ++ usersInfo(d)

    private def basicInfo(d: ClusterDetails) = Json.obj(
      "href" -> d.href,
      "id" -> d.id,
      "name" -> d.name,
      "size" -> d.size,
      "state" -> d.state,
      "stateDescription" -> d.stateDescription
    )

    private def machinesInfo(d: ClusterDetails) = (d.master, d.slaves) match {
      case (Some(masterDetails), Some(slavesDetails)) => Json.obj(
        "master" -> masterDetails,
        "slaves" -> slavesDetails
      )
      case _ => Json.obj()
    }

    private def usersInfo(d: ClusterDetails) = d.users match {
      case Some(users) => Json.obj(
        "users" -> users
      )
      case _ => Json.obj()
    }
  }
}
