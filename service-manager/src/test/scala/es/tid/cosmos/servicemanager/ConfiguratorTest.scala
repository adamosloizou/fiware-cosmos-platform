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

package es.tid.cosmos.servicemanager

import org.mockito.BDDMockito.given
import org.mockito.Mockito.{verify, verifyNoMoreInteractions}
import org.mockito.Matchers.any
import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.ambari.{Host, Cluster}
import es.tid.cosmos.servicemanager.Configurator.ConfigurationConflict
import es.tid.cosmos.servicemanager.ConfiguratorTestHelpers.contributionsWithNumber

/**
 * @author adamos
 */
class ConfiguratorTest extends FlatSpec with OneInstancePerTest with MustMatchers with MockitoSugar {

  val cluster = mock[Cluster]
  val master = mock[Host]

  "A Configurator" must "not apply configuration when there is none available" in {
    Configurator.applyConfiguration(cluster, master, contributors = List())
    verifyNoMoreInteractions(cluster, master)
  }

  it must "configure using only one contributor" in {
    val contributor = mock[ConfigurationContributor]
    given(contributor.contributions(any())).willReturn(contributionsWithNumber(1))
    Configurator.applyConfiguration(cluster, master, List(contributor))
    verify(cluster).applyConfiguration(contributionsWithNumber(1).global.get)
    verify(cluster).applyConfiguration(contributionsWithNumber(1).core.get)
    verify(cluster).applyConfiguration(contributionsWithNumber(1).services(0))
  }

  it must "configure using multiple non-overlapping contributors" in {
    val contributor1 = mock[ConfigurationContributor]
    val contributor2 = mock[ConfigurationContributor]
    given(contributor1.contributions(any())).willReturn(contributionsWithNumber(1))
    given(contributor2.contributions(any())).willReturn(contributionsWithNumber(2))
    Configurator.applyConfiguration(cluster, master, List(contributor1, contributor2))
    verify(cluster).applyConfiguration(GlobalConfiguration("version1",
      Map("someGlobalContent1" -> "somevalue1", "someGlobalContent2" -> "somevalue2")))
    verify(cluster).applyConfiguration(CoreConfiguration("version1",
        Map("someCoreContent1" -> "somevalue1", "someCoreContent2" -> "somevalue2")))
    verify(cluster).applyConfiguration(ServiceConfiguration("service-site1", "version1", Map("someServiceContent1" -> "somevalue1")))
    verify(cluster).applyConfiguration(ServiceConfiguration("service-site2", "version1", Map("someServiceContent2" -> "somevalue2")))
  }

  it must "fail when more than one service configurations have the same type" in {
    val contributor1 = mock[ConfigurationContributor]
    val contributor2 = mock[ConfigurationContributor]
    val configuration1 = contributionsWithNumber(1)
    val configuration2 = contributionsWithNumber(2)
    given(contributor1.contributions(any())).willReturn(configuration1)
    given(contributor2.contributions(any())).willReturn(
      ConfigurationBundle(configuration2.global, configuration2.core, configuration1.services))
    evaluating {
      Configurator.applyConfiguration(cluster, master, List(contributor1, contributor2))
    } must produce [ConfigurationConflict]
  }

  it must "fail when global configuration contributions have property conflicts" in {
      val contributor1 = mock[ConfigurationContributor]
      val contributor2 = mock[ConfigurationContributor]
      val configuration1 = contributionsWithNumber(1)
      val configuration2 = contributionsWithNumber(2)
      given(contributor1.contributions(any())).willReturn(configuration1)
      given(contributor2.contributions(any())).willReturn(
        ConfigurationBundle(configuration1.global, configuration2.core, configuration2.services))
      evaluating {
        Configurator.applyConfiguration(cluster, master, List(contributor1, contributor2))
      } must produce [ConfigurationConflict]
    }
}
