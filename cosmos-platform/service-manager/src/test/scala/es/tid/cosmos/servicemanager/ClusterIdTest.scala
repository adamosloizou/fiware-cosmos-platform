package es.tid.cosmos.servicemanager

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

/**
 * @author sortega
 */
class ClusterIdTest extends FlatSpec with MustMatchers {

  "ClusterId instances" must "be equal by value" in {
    val instance0 = ClusterId("00000000-0000-0000-0000-000000000000")
    val instance1 = ClusterId("00000000-0000-0000-0000-000000000000")
    instance0 must equal (instance1)
    instance0.hashCode() must equal (instance1.hashCode())
  }

  "ClusterId of random UUID" must "be equal by value with ClusterId with same UUID" in {
    val instance0 = ClusterId()
    val instance1 = ClusterId(instance0.uuid)
    instance0 must equal (instance1)
    instance0.hashCode() must equal (instance1.hashCode())
  }
}
