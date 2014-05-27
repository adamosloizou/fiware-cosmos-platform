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

import java.text.SimpleDateFormat
import java.util.Date

import com.typesafe.sbt.SbtNativePackager
import com.typesafe.sbt.SbtNativePackager._
import com.typesafe.sbt.packager.Keys._
import com.typesafe.sbt.packager.linux.LinuxSymlink
import sbt._
import sbt.Keys._
import sbtassembly.Plugin._
import sbtassembly.Plugin.AssemblyKeys._

object RpmSettings {

  val currentTimestamp = new SimpleDateFormat(".yyyyMMdd.HHmmss").format(new Date)

  val commonRpmSettings: Seq[Setting[_]] = Seq(
    dist <<= (packageBin in Rpm) dependsOn dist,
    maintainer := "Cosmos Team <cosmos@tid.es>",
    rpmVendor := "tefdigital",
    rpmGroup := Some("System Environment/Libraries"),
    rpmLicense := Some("All rights reserved"),
    version in Rpm <<= version apply { v =>
      v.replace("-SNAPSHOT", currentTimestamp)
    },
    packageArchitecture in Rpm := "noarch",
    rpmOs in Rpm:= "linux",
    rpmRequirements := Seq("java"),
    rpmAutoreq := "no",
    rpmAutoprov := "no",
    rpmBrpJavaRepackJars := true
  )

  lazy val CosmosCli = config("cosmosCli")

  val CosmosApiPath = "/opt/pdi-cosmos/cosmos-api/"

  val cosmosApiSettings: Seq[Setting[_]] = commonRpmSettings ++ Seq(
    name in Rpm := "cosmos-api",
    packageSummary := "Cosmos API",
    packageDescription in Rpm := "Cosmos API server",
    packageBin in CosmosCli := {
      CosmosCliBuildHelper.make(baseDirectory.value.getParentFile / "cosmos-cli", streams.value.log)
    },
    linuxPackageMappings in Rpm <<= (mappings in Universal) map { (f: Seq[(File,String)]) =>
      f map { case (file: File, name: String) =>
        (packageMapping(file -> s"$CosmosApiPath/$name")
          withUser "root" withGroup "root" withPerms "0755")
      }
    },
    linuxPackageMappings in Rpm <++= baseDirectory map { base =>
      Seq(
        packageMapping(base / "scripts/cosmos-api" -> "/etc/init.d/cosmos-api"),
        packageMapping(IO.temporaryDirectory / "." -> "/opt/pdi-cosmos/var/run/")
          withUser "root" withGroup "root" withPerms "0440"
      )
    },
    linuxPackageMappings in Rpm <+= (baseDirectory, packageBin in CosmosCli) map { (base, egg) =>
      packageMapping(egg -> s"/opt/repos/eggs/${egg.name}")
    },
    linuxPackageSymlinks := {
      Seq(LinuxSymlink("/usr/bin/cosmos-api", s"$CosmosApiPath/bin/cosmos-api"))
    },
    rpmPost := Some("""chmod +x /etc/init.d/cosmos-api
                      |chkconfig --add /etc/init.d/cosmos-api
                      |chkconfig cosmos-api on
                    """.stripMargin)
  )

  val CosmosAdminPath = "/opt/pdi-cosmos/cosmos-admin"

  val cosmosAdminSettings: Seq[Setting[_]] =
    SbtNativePackager.packageArchetype.java_application ++ commonRpmSettings ++
    Seq(
      name in Rpm := "cosmos-admin",
      packageSummary := "Cosmos Admin",
      packageDescription in Rpm := "Cosmos Admin server",
      linuxPackageMappings in Rpm <<= (mappings in Universal) map { (files: Seq[(File,String)]) =>
        for {
          (file, name) <- files if name.endsWith(".jar")
        } yield {
          (packageMapping(file -> s"$CosmosAdminPath/lib/${file.name}")
            withUser "root" withGroup "root" withPerms "0755")
        }
      },
      linuxPackageMappings in Rpm <++= baseDirectory map { base =>
        Seq(
          packageMapping (base / "scripts/cosmos-admin" -> s"$CosmosAdminPath/cosmos-admin")
            withUser "root" withGroup "root" withPerms "0755",
          packageMapping (IO.temporaryDirectory / "." -> s"$CosmosAdminPath/logs")
            withUser "root" withGroup "root" withPerms "0755"
        )
      },
      linuxPackageSymlinks := {
        Seq(LinuxSymlink("/usr/bin/cosmos-admin", s"$CosmosAdminPath/cosmos-admin"))
      }
    )

  val JarPath = "/opt/pdi-cosmos/infinity-driver.jar"
  val JarLinks = for (service <- Seq("hdfs", "mapreduce", "yarn"))
    yield s"/usr/lib/hadoop-$service/lib/infinity-driver.jar"

  val infinityDriverSettings: Seq[Setting[_]] =
    assemblySettings ++ packagerSettings ++ commonRpmSettings ++ Seq(
      name in Rpm := "infinity-driver",
      packageSummary := "Infinity HFS driver",
      packageDescription in Rpm := "Library that enables the infinity:// scheme.",
      linuxPackageMappings in Rpm := Seq(
        packageMapping(assembly.value -> JarPath) withUser "root" withGroup "root" withPerms "0755"
      ),
      linuxPackageSymlinks in Rpm := JarLinks.map(destination => LinuxSymlink(destination, JarPath))
    )

  val InfinityServerPath = "/usr/lib/hadoop/lib"

  val infinityServerSettings: Seq[Setting[_]] =
    assemblySettings ++ packagerSettings ++ commonRpmSettings ++ Seq(
      name in Rpm := "infinity-server",
      packageSummary := "Infinity Server",
      packageDescription in Rpm := "Server for infinity:// scheme.",
      rpmRequirements := Seq("hadoop-hdfs"),
      linuxPackageMappings in Rpm := Seq(
        packageMapping(assembly.value -> s"$InfinityServerPath/infinity-server.jar")
          withUser "root" withGroup "root" withPerms "0755"
      )
    )
}
