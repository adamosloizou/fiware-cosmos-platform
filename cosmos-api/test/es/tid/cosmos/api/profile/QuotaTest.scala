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

package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.quota.{Quota, FiniteQuota, UnlimitedQuota, EmptyQuota}

class QuotaTest extends FlatSpec with MustMatchers {

  "An empty quota" must "reject every request" in {
    EmptyQuota.withinQuota(4) must be (false)
    EmptyQuota.withinQuota(1000) must be (false)
  }

  "An unlimited quota" must "accept every request" in {
    UnlimitedQuota.withinQuota(4) must be (true)
    UnlimitedQuota.withinQuota(1000) must be (true)
  }

  "A finite quota" must "accept request within bounds" in {
    FiniteQuota(5).withinQuota(EmptyQuota) must be (true)
    FiniteQuota(5).withinQuota(4) must be (true)
  }

  it must "be valid only for positive values" in {
    evaluating{ FiniteQuota(0) } must produce [IllegalArgumentException]
    evaluating{ FiniteQuota(-1) } must produce [IllegalArgumentException]
  }

  it must "disallow requests out of bounds" in {
    FiniteQuota(5).withinQuota(1000) must be (false)
    FiniteQuota(5).withinQuota(UnlimitedQuota) must be (false)
  }

  "A quota" must "be initialized from optional value" in {
    Quota(None) must equal (UnlimitedQuota)
    Quota(Some(0)) must equal (EmptyQuota)
    Quota(Some(5)) must equal (FiniteQuota(5))
  }

  it must "be convertible to a numeric representation" in {
    UnlimitedQuota.toOptInt must equal (None)
    EmptyQuota.toOptInt must equal (Some(0))
    FiniteQuota(5).toOptInt must equal (Some(5))
  }

  "The max quota between an empty and another quota" must "be the other quota" in {
    Quota.max(EmptyQuota, EmptyQuota) must equal (EmptyQuota)
    Quota.max(EmptyQuota, UnlimitedQuota) must equal (UnlimitedQuota)
    Quota.max(EmptyQuota, FiniteQuota(1)) must equal (FiniteQuota(1))
  }

  "The min quota between an empty and another quota" must "be the empty quota" in {
    Quota.min(EmptyQuota, EmptyQuota) must equal (EmptyQuota)
    Quota.min(EmptyQuota, UnlimitedQuota) must equal (EmptyQuota)
    Quota.min(EmptyQuota, FiniteQuota(1)) must equal (EmptyQuota)
  }

  "The max quota between an unlimited and another quota" must "be the unlimited quota" in {
    Quota.max(UnlimitedQuota, UnlimitedQuota) must equal (UnlimitedQuota)
    Quota.max(UnlimitedQuota, EmptyQuota) must equal (UnlimitedQuota)
    Quota.max(EmptyQuota, UnlimitedQuota) must equal (UnlimitedQuota)
    Quota.max(UnlimitedQuota, FiniteQuota(1)) must equal (UnlimitedQuota)
    Quota.max(FiniteQuota(1), UnlimitedQuota) must equal (UnlimitedQuota)
  }

  "The min quota between an unlimited and another quota" must "be the other quota" in {
    Quota.min(UnlimitedQuota, UnlimitedQuota) must equal (UnlimitedQuota)
    Quota.min(UnlimitedQuota, EmptyQuota) must equal (EmptyQuota)
    Quota.min(UnlimitedQuota, FiniteQuota(1)) must equal (FiniteQuota(1))
  }

  "The min quota between two finite quotas" must "be the one with lower limit" in {
    Quota.min(FiniteQuota(1), FiniteQuota(2)) must equal(FiniteQuota(1))
    Quota.min(FiniteQuota(2), FiniteQuota(1)) must equal(FiniteQuota(1))
  }

  "Adding two quotas" must "have empty and unlimited as upper and lower bounds" in {
    EmptyQuota     + EmptyQuota must equal(EmptyQuota)
    EmptyQuota     + FiniteQuota(1) must equal(FiniteQuota(1))
    EmptyQuota     + UnlimitedQuota must equal(UnlimitedQuota)
    UnlimitedQuota + EmptyQuota must equal(UnlimitedQuota)
    UnlimitedQuota + FiniteQuota(1) must equal(UnlimitedQuota)
    UnlimitedQuota + UnlimitedQuota must equal(UnlimitedQuota)
    FiniteQuota(1) + EmptyQuota must equal(FiniteQuota(1))
    FiniteQuota(1) + FiniteQuota(2) must equal(FiniteQuota(3))
    FiniteQuota(1) + UnlimitedQuota must equal(UnlimitedQuota)
  }

  "Subtracting one quota from another" must "have empty and unlimited as upper and lower bounds" in {
    EmptyQuota     - EmptyQuota must equal(EmptyQuota)
    EmptyQuota     - FiniteQuota(1) must equal(EmptyQuota)
    EmptyQuota     - UnlimitedQuota must equal(EmptyQuota)
    UnlimitedQuota - EmptyQuota must equal(UnlimitedQuota)
    UnlimitedQuota - FiniteQuota(1) must equal(UnlimitedQuota)
    evaluating { UnlimitedQuota - UnlimitedQuota } must produce[IllegalArgumentException]
    FiniteQuota(1) - EmptyQuota must equal(FiniteQuota(1))
    FiniteQuota(3) - FiniteQuota(1) must equal(FiniteQuota(2))
    FiniteQuota(1) - FiniteQuota(2) must equal(EmptyQuota)
    FiniteQuota(1) - UnlimitedQuota must equal(EmptyQuota)
  }
}
