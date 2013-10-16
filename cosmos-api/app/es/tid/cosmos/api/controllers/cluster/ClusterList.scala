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

import play.api.libs.json.{Json, JsValue, Writes}

/**
 * List of clusters as seen by the client.
 */
case class ClusterList(clusters: Seq[AbsoluteUriClusterReference])

object ClusterList {
  implicit object ClusterListWrites extends Writes[ClusterList] {
    def writes(list: ClusterList): JsValue = Json.obj(
      "clusters" -> list.clusters.map(ref => Json.toJson(ref))
    )
  }
}
