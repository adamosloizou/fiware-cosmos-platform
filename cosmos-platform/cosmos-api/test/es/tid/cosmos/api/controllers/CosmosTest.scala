package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test._
import play.api.test.Helpers._

/**
 * @author sortega
 */
class CosmosTest extends FlatSpec with MustMatchers {

  "The Cosmos resource" must "return the version as JSON" in {
    running(FakeApplication()) {
      val resource = route(FakeRequest(GET, "/cosmos")).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      contentAsString(resource) must include (Cosmos.apiVersion)
    }
  }
}
