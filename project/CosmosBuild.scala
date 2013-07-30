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

import sbt._

object CosmosBuild extends Build {

  lazy val root = (Project(id = "cosmos-platform", base = file("."))
    settings(ScctPlugin.mergeReportSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    aggregate(cosmosApi, serviceManager, ial, cosmosAdmin, common)
  )

  lazy val common = (Project(id = "common", base = file("common"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
  )

  lazy val common_test = (Project(id = "common-test", base = file("common-test"))
    settings(Defaults.itSettings : _*)
    configs(IntegrationTest)
    dependsOn(common)
  )

  lazy val ial = (Project(id = "ial", base = file("ial"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(common, common_test % "compile->compile;test->test")
  )

  lazy val serviceManager = (Project(id = "service-manager", base = file("service-manager"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(ial, common_test % "compile->compile;test->test")
  )

  lazy val cosmosApi = (play.Project("cosmos-api", "0.10.1", path = file("cosmos-api"),
                        dependencies = Seq(PlayKeys.anorm, PlayKeys.jdbc))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(serviceManager)
  )

  lazy val cosmosAdmin = (Project(id = "cosmos-admin", base = file("cosmos-admin"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(serviceManager, common)
  )
}
