package es.tid.cosmos.api.profile


import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar
import org.mockito.BDDMockito._

import es.tid.cosmos.servicemanager.ClusterDescription
import es.tid.cosmos.api.auth.ApiCredentials
import es.tid.cosmos.api.controllers.cluster.ClusterReference

class ProfileQuotasTest extends FlatSpec with MustMatchers with MockitoSugar {

  "A user request exceeding their personal quota" must "not be allowed" in {
    val userGroup = GuaranteedGroup("A", FiniteQuota(3))
    val groupUsage = Map[Group, Int](
      userGroup -> 2,
      GuaranteedGroup("B", FiniteQuota(2)) -> 0,
      NoGroup -> 3
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = FiniteQuota(3),
      usedMachinesByUser = 2,
      machinePool = 10
    )
    withConfig(context) { (quotas, profile) =>
      quotas.withinQuota(profile, 1).isSuccess must be(true)
      quotas.withinQuota(profile, 2).isSuccess must be(false)
    }
  }

  "A user request exceeding their minimum group quota" must "be allowed if machines available" in {
    /*
    +----+----+---+---+----+----+----+---+---+----+
    | 1  | 2  | 3 | 4 | 5  | 6  | 7  | 8 | 9 | 10 |
    +----+----+---+---+----+----+----+---+---+----+
    | A  | A  | B | B | NG | NG | NG | o | o | o  |
    | ur | ur | r | r | u  | u  | u  | o | o | o  |
    +----+----+---+---+----+----+----+---+---+----+
                                       X   X   X
     */
    val userGroup = GuaranteedGroup("A", FiniteQuota(2))
    val groupUsage = Map[Group, Int](
      userGroup -> 2,
      GuaranteedGroup("B", FiniteQuota(2)) -> 0,
      NoGroup -> 3
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = UnlimitedQuota,
      usedMachinesByUser = 2,
      machinePool = 10
    )
    withConfig(context) { (quotas, profile) =>
      quotas.withinQuota(profile, 1).isSuccess must be(true)
      quotas.withinQuota(profile, 2).isSuccess must be(true)
      quotas.withinQuota(profile, 3).isSuccess must be(true)
      quotas.withinQuota(profile, 4).isSuccess must be(false)
    }
  }

  "A user without personal quota" must "be allowed the max number of available machines" in {
    /*
    +----+----+---+---+---+----+----+----+---+----+
    | 1  | 2  | 3 | 4 | 5 | 6  | 7  | 8  | 9 | 10 |
    +----+----+---+---+---+----+----+----+---+----+
    | A  | A  | A | B | B | NG | NG | NG | o | o  |
    | ur | ur | r | r | r | u  | u  | u  | o | o  |
    +----+----+---+---+---+----+----+----+---+----+
                X                          X   X
     */
    val userGroup = GuaranteedGroup("A", FiniteQuota(3))
    val groupUsage = Map[Group, Int](
      userGroup -> 2,
      GuaranteedGroup("B", FiniteQuota(2)) -> 0,
      NoGroup -> 3
    )
    val context = Context(
      groupUsage,
      userGroup,
      personalMaxQuota = UnlimitedQuota,
      usedMachinesByUser = 2,
      machinePool = 10
    )
    withConfig(context){ (quotas, profile) =>
      quotas.withinQuota(profile, 3).isSuccess must be(true)
      quotas.withinQuota(profile, 4).isSuccess must be(false)
    }
  }


  def withConfig(c: Context)
              (test: (ProfileQuotas, CosmosProfile) => Unit) {
    // for each group we need
    // 1. Create 1 profile or 1 + user for userGroup
    // 2. For each profile we need to create one cluster with the size of the group's usage
    // 3. Register the profiles
    // 4. assign clusters

    val groupProfiles = (for ((group, id) <- c.groupUsage.keys zip (1 to c.groupUsage.size)) yield {
      val quota = if (group == c.userGroup) c.personalMaxQuota else UnlimitedQuota
      val profile = CosmosProfile(id, s"handle$id", group, quota, ApiCredentials.random(), keys = Nil)
      group -> Set(profile)
    }).toMap

    val groupClusters = c.groupUsage.map{ case (group, used) => {
      val description = mock[ClusterDescription]
      given(description.size).willReturn(used)
      val reference = mock[ClusterReference]
      given(reference.description).willReturn(description)
      groupProfiles(group).head -> List(reference)
    }}

    val quotas = new ProfileQuotas(
      machinePoolSize = c.machinePool,
      groups = c.groupUsage.keys.toSet,
      lookupByGroup = groupProfiles,
      listClusters = groupClusters
    )

    test(quotas, groupProfiles(c.userGroup).head)
  }

  case class Context(
    groupUsage: Map[Group, Int],
    userGroup: Group,
    personalMaxQuota: Quota,
    usedMachinesByUser: Int,
    machinePool: Int
  )
}
