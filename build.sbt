name := "cosmos-platform"

organization in ThisBuild := "es.tid.cosmos"

// Note: This is the Scala version used by Play 2.2.2. Ensure Play compatibility before upgrading.
scalaVersion in ThisBuild := "2.10.3"

scalacOptions in ThisBuild ++= Seq("-deprecation", "-feature")

javacOptions in ThisBuild ++= Seq("-source", "1.7")

addCommandAlias("run-local-it", ";it:compile ;it:test-only * -- -l \"HasExternalDependencies EndToEndTest\"")

resolvers in ThisBuild ++= Seq(
    DefaultMavenRepository,
    "Cosmos Nexus Repository" at "http://cosmos10/nexus/content/groups/public/",
    "Typesafe Repository" at "http://repo.typesafe.com/typesafe/releases/",
    Resolver.url("Play", url("http://download.playframework.org/ivy-releases/"))(Resolver.ivyStylePatterns)
)

libraryDependencies in ThisBuild ++= Seq(
  Dependencies.mockito % "test, it",
  Dependencies.scalatest % "test, it"
)
