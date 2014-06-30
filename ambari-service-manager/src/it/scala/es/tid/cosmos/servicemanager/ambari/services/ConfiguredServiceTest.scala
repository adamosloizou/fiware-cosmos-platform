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

package es.tid.cosmos.servicemanager.ambari.services

import java.io.File
import java.nio.file.{StandardCopyOption, Files}

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.resources.TestResourcePaths
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationContributor
import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys

trait ConfiguredServiceTest extends FlatSpec with MustMatchers with TestResourcePaths {

  def dynamicProperties: Map[ConfigurationKeys.Value, String]
  def configurator: ConfigurationContributor

  def contributions = configurator.contributions(dynamicProperties)

  ConfiguredServiceTest.synchronized {
    new File(resourcesConfigDirectory)
      .listFiles()
      .filter(_.getName.endsWith(".erb"))
      .foreach(file => {
        val path = file.getAbsolutePath
        Files.move(
          file.toPath,
          new File(path.take(path.lastIndexOf('.'))).toPath,
          StandardCopyOption.REPLACE_EXISTING)
      })
  }
}

object ConfiguredServiceTest
