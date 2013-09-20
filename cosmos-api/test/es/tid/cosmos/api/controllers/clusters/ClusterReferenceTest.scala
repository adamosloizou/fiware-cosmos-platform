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

package es.tid.cosmos.api.controllers.clusters

import java.util.Date

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.Json

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.api.profile.ClusterAssignation

class ClusterReferenceTest extends FlatSpec with MustMatchers {

  object TestDescription extends ClusterDescription {
    override val id = ClusterId("clusterId")
    override val name = "clusterName"
    override val size = 10
    override val state = Provisioning
    override def nameNode_> = throw new NotImplementedError()
    override def master_> = throw new NotImplementedError()
    override def slaves_> = throw new NotImplementedError()
  }

  "A cluster reference" must ("be composed of cluster description and assignation" +
    " for the same cluster id") in {
    val assignation = ClusterAssignation(ClusterId("otherId"), 1L, new Date())
    evaluating {
      ClusterReference(TestDescription, assignation)
    } must produce [IllegalArgumentException]
  }

  "A cluster reference with an absolute URL" must "be convertible to JSON" in {
    val assignation = ClusterAssignation(TestDescription.id, 1L, new Date(0))
    val reference = ClusterReference(TestDescription, assignation)
    val url = "http://host/resource"
    Json.toJson(AbsoluteUriClusterReference(url, reference)) must equal (Json.obj(
      "id" -> "clusterId",
      "href" -> url,
      "name" -> "clusterName",
      "state" -> "provisioning",
      "stateDescription" -> "Cluster is acquiring and configuring resources",
      "creationDate" -> "1970-01-01T01:00:00+01:00"
    ))
  }
}
