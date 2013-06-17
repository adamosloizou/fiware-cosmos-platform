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

package es.tid.cosmos.servicemanager.ambari.configuration

import scala.collection.JavaConversions.mapAsScalaMap

import com.typesafe.config.{ConfigFactory, Config}

import es.tid.cosmos.servicemanager.ambari.configuration.FactoryTypes.Factory
import es.tid.cosmos.servicemanager.ambari.configuration.FactoryTypes.Implicits._

/**
 * Trait for mixing-in configuration loading capabilities.
 * The loader will load the configuration found in the classpath `[configName].conf` file
 * as specified by `configName`.
 *
 * The Loader makes use of the HOCON configuration format.
 *
 * @see [[es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationBuilder]]
 * @see [[https://github.com/typesafehub/config/blob/master/HOCON.md About HOCON format]]
 */
trait ConfigurationLoader {
  /**
   * Load the configuration with the given name.
   * It will look for a classpath resource file `[configName].conf`
   *
   * @param configName the configuration name
   * @return the configuration builder that can be then used to inject runtime values
   *         before fully constructing the configuration data
   */
  def load(configName: String) = {
    val name = configName.toLowerCase
    ConfigurationBuilder(name, ConfigFactory.load(classOf[ConfigurationLoader].getClassLoader, name))
  }
}

/**
 * Builder of service description configurations.
 * For a given service and its configuration it looks for 3 types of configuration:
 * Global, Core and Service. They are all optional.
 *
 * configuration example:
 * {{{
 *"global" {
 *  "tag"="aTag",
 *  "properties" {
 *    "global.example.string"="global-${masternode}"
 *   }
 *}
 *"core" {
 *  "tag"="aTag",
 *  "properties" {
 *    "core.example"="core-${masternode}"
 *   }
 *}
 *"a-service" {
 *  "configType"="test-service-site"
 *  "tag"="aTag",
 *  "properties" {
 *    "service.example"="service-${masternode}"
 *   }
 *}
 * }}}
 *
 * @param serviceName the service name. It will be used in lowercase to lookup its configuration
 * @param config the configuration for this service
 */
case class ConfigurationBuilder(serviceName: String, config: Config) {
  /**
   * Get the value of a configuration path comprising of its parent and leaf paths.
   *
   * ''Note'': this employs ''currying'' and needs to be ''defined before'' being used by
   * any val definitions that use it.
   *
   * @param leaf the last part of the configuration path
   * @param parentPath the configuration path's parent path
   * @return the value for the given assembled path
   */
  private def valueOf(leaf: String)(parentPath: String) = config.getString(s"$parentPath.$leaf")

  private val configType = valueOf("configType") _

  /**
   * Build the final configuration injecting values to configurations that contain certain
   * placeholders.
   *
   * @param properties the values that will be injected into the placeholders
   * @return the configuration with master-node name injected
   */
  def build(properties: Map[ConfigurationKeys.Value, String]): ConfigurationBundle =
    ConfigurationBundle(
      optional[GlobalConfiguration]("global", properties),
      optional[CoreConfiguration]("core", properties),
      service(properties)
    )

  private def optional[T <: Configuration](
      name: String,
      properties: Map[ConfigurationKeys.Value, String])
      (implicit factory: Factory[T]): Option[T] =
    resolveProperties(name, properties).map(props => factory(props))

  private def service(properties: Map[ConfigurationKeys.Value, String]) =
    resolveProperties(serviceName, properties)
      .map(props => ServiceConfiguration(configType(serviceName), props))
      .toList

  private def resolveProperties(
      configName: String,
      properties: Map[ConfigurationKeys.Value, String]) =
    getScalaMap(config, s"$configName.properties").map(_.mapValues(inject(properties)))

  private def getScalaMap(config: Config, key: String) =
    if (config.hasPath(key))
      Some(mapAsScalaMap(config.getObject(key).unwrapped()).toMap)
    else None

  private def inject(properties: Map[ConfigurationKeys.Value, String])(target: Any) = target match {
    case x: String => properties.foldLeft(x)((str, entry) =>
      str.replaceAll("\\$\\{" + entry._1.toString +"\\}", entry._2))
    case other => other
  }
}
