name := "infinity-server"

description := "Infinity Server"

libraryDependencies ++= Dependencies.akka ++ Dependencies.spray ++ Seq(
  Dependencies.anorm,
  Dependencies.commonsCodec,
  Dependencies.finatra,
  Dependencies.h2database % "test",
  Dependencies.hadoopCommon % "provided",
  Dependencies.hadoopHdfs % "provided",
  Dependencies.liftJson,
  Dependencies.logbackClassic,
  Dependencies.scalalikejdbc,
  Dependencies.scalaMigrations,
  Dependencies.scalaz,
  Dependencies.typesafeConfig
)

seq(Revolver.settings: _*)
