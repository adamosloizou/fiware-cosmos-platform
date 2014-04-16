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

package es.tid.cosmos.api.controllers

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test._
import play.api.test.Helpers._

import es.tid.cosmos.api.mocks.WithTestApplication

class ServicesIT extends FlatSpec with MustMatchers {
  "Services listing" must "list services provided by Service Manager" in new WithTestApplication {
    val resource = route(FakeRequest(GET, "/cosmos/v1/services")).get
    status(resource) must equal (OK)
    contentType(resource) must be (Some("application/json"))
    contentAsString(resource) must include ("PIG")
  }
}
